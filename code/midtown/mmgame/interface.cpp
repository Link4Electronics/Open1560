/*
    Open1560 - An Open Source Re-Implementation of Midtown Madness 1 Beta
    Copyright (C) 2020 Brick

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

define_dummy_symbol(mmgame_interface);

#include "interface.h"

#include <fcntl.h>
#include <unistd.h>

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>

#include "eventq7/event.h"
#include "pcwindis/dxinit.h"
#include "agi/rsys.h"
#include "mmui/driver.h"
#include "mmui/main.h"
#include "mmui/options.h"
#include "mmui/placeholder_opts.h"
#include "mmui/vehicle.h"
#include "mmui/quitmenu.h"
#include "mmwidget/navbar.h"
#include "agisw/swrend.h"
#include "agiworld/quality.h"
#include "agiworld/texsheet.h"
#include "data7/memstat.h"
#include "data7/timer.h"
#include "memory/allocator.h"
#include "midtown.h"
#include "mmaudio/manager.h"
#include "mmcar/carsim.h"
#include "mmcityinfo/citylist.h"
#include "mmcityinfo/playerdata.h"
#include "mmcityinfo/state.h"
#include "mmcityinfo/vehlist.h"
#include "mmnetwork/network.h"
#include "mmwidget/manager.h"
#include "mmwidget/menu.h"
#include "mmwidget/bm_button.h"

// ?IsModemDialin@@YA_NXZ
ARTS_IMPORT /*static*/ bool IsModemDialin();

// ?ZoneWatcher@@YGKPAX@Z
ARTS_IMPORT /*static*/ ulong ARTS_STDCALL ZoneWatcher(void* arg1);

mmInterface::mmInterface()
{
    void* saved_vtable = *reinterpret_cast<void**>(this);
    std::memset(this, 0, sizeof(*this));
    *reinterpret_cast<void**>(this) = saved_vtable;

    ActivateNode();

    // Initialize ARTS_IMPORT globals to nullptr to avoid deleting sentinel values
    // from the game binary on early-exit cleanup paths
    CityListPtr = nullptr;
    VehicleListPtr = nullptr;
}

mmInterface::~mmInterface()
{
    agiCurState.SetTexFilter(agiRQ.TexFilter ? agiTexFilter::Trilinear : agiTexFilter::Bilinear);
    swSetInterlace(MMSTATE.Interlaced);
    EnableSmoke = ForceSmoke | !agiCurState.GetSoftwareRendering();
    ALLOCATOR.SanityCheck();

    {
        ARTS_MEM_STAT("mmInterface Destructor");

        AudMgr()->DeallocateUIADF();
        NETMGR.Deallocate();

        if (MenuManager::Instance)
        {
            delete MenuManager::Instance;

            MenuManager::Instance = nullptr;
        }

        std::memset(PlayerIDs, 0, sizeof(PlayerIDs));

        ALLOCATOR.SanityCheck();
    }

    TEXSHEET.Kill();

    if (VehicleListPtr)
    {
        delete VehicleListPtr;
        VehicleListPtr = nullptr;
    }

    if (CityListPtr)
    {
        delete CityListPtr;
        CityListPtr = nullptr;
    }

    ALLOCATOR.SanityCheck();
}

void mmInterface::SetStateRace(i32 /*arg1*/)
{}

void ReportTimeAlloc(f32 time)
{
    Displayf(
        "*********Load time %f = %f seconds, %dK Allocated", time, LoadTimer.Time(), ALLOCATOR.GetHeapUsed() >> 10);
}

// ?JoinViaZone@@3HA
ARTS_IMPORT extern b32 JoinViaZone;

void mmInterface::InitLobby()
{
    NETMGR.InitializeLobby(8, false);

    if (!JoinViaZone)
    {
        if (NETMGR.JoinLobbySession())
        {
            NETMGR.SetSysCallback([this](void* param) { MessageCallback(nullptr, param); });
            NETMGR.SetAppCallback([this](void* param) { MessageCallback2(nullptr, param); });
        }
        else
        {
            MMSTATE.NetworkStatus = 0;
            JoinViaZone = true;
        }
    }
}

void mmInterface::PlayerResolveCars()
{
    // NOTE: Use MMCURRPLAYER to check passes/score, because PlayerResolveScore may not have been called yet.

    mmVehList* vehlist = VehList();
    mmCityInfo* cityinfo = DefaultCityInfo();

    for (i32 i = 0; i < vehlist->NumVehicles; ++i)
    {
        mmVehInfo* vehinfo = vehlist->GetVehicleInfo(i);
        bool locked = false;

        if (u32 flags = vehinfo->UnlockFlags)
        {
            if (flags & VEH_INFO_UNLOCK_ANY_TWO)
                locked = locked || (MMCURRPLAYER.GetTotalPassed() < 2);

            if (flags & VEH_INFO_UNLOCK_BLITZ)
                locked = locked || (MMCURRPLAYER.GetBlitzPassed() < cityinfo->BlitzCount / 2);

            if (flags & VEH_INFO_UNLOCK_CIRCUIT)
                locked = locked || (MMCURRPLAYER.GetCircuitPassed() < cityinfo->CircuitCount / 2);

            if (flags & VEH_INFO_UNLOCK_CHECKPOINT)
                locked = locked || (MMCURRPLAYER.GetCheckpointPassed() < cityinfo->CheckpointCount / 2);

            if (flags & VEH_INFO_UNLOCK_COMPLETE)
                locked = locked ||
                    (MMCURRPLAYER.GetTotalPassed() <
                        cityinfo->BlitzCount + cityinfo->CircuitCount + cityinfo->CheckpointCount);

            if (flags & VEH_INFO_UNLOCK_PRO)
                locked = locked || (MMCURRPLAYER.Difficulty != 1);
        }

        if (i32 score = vehinfo->UnlockScore)
            locked = locked || (MMCURRPLAYER.GetTotalScore() < score);

        vehinfo->IsLocked = locked && !AllCars;
    }
}

void mmInterface::SetStateDefaults()
{
    MMSTATE.TimeOfDay = mmTimeOfDay::Noon;
    MMSTATE.GameMode = mmGameMode::Cruise;
    MMSTATE.Weather = mmWeather::Sun;
    MMSTATE.EventId = 0;
}

void mmInterface::SetNavigationOrders()
{
    // FIXME: Requires MenuManager::Instance to be set up with all menus constructed.
    // Weak stubs skip initialization, so all pointers are null/garbage.
    if (!MenuMgr())
        return;

    // TODO: Do this during menu construction instead
    const auto fixup = [this](UIMenu* menu, std::initializer_list<const char*> labels) {
        if (menu)
            menu->SetNavigationOrder(labels.begin(), labels.size());
    };

    fixup((UIMenu*) MenuMgr()->GetNavBar(), {"mnav_prev", "mnav_opt", "mnav_help", "mnav_stow", "mnav_exit"});
    fixup((UIMenu*) MenuRace,
        {"race_roam", "race_blitz", "race_waypt", "race_circ", "RACE NAME", "race_drop_frame", "race desc icons",
            "race_cenv", "LAPS", "race_laps", "CHECKPOINTS", "race_checkpoints", "OPPONENTS", "race_oppo", "race_env",
            "TOD Icons", "Weather Icons", "TRAFFIC DENSITY", "PEDESTRIAN DENSITY", "COP DENSITY", "race_next"});
    fixup((UIMenu*) MenuHostRace,
        {"race_roam", "race_blitz", "race_waypt", "race_circ", "race_cops", "RACE NAME", "host_drop_frame",
            "race desc icons", "LAPS", "host_laps", "CHECKPOINTS", "race_checkpoints", "host_checkpoints", "OPPONENTS",
            "race_oppo", "race_cenv", "Password", "Max Players", "race_env", "TOD Icons", "Weather Icons",
            "PEDESTRIAN DENSITY", "host_cont"});

    fixup((UIMenu*) DlgDriverRec, {"compscroll", "drec_bltz", "drec_circ", "drec_chck", "dlg_done"});

    fixup((UIMenu*) DlgHallOfFame,
        {"compscroll", "drec_bltz", "drec_circ", "drec_chck", "hoff_amap", "hoff_prop", "hoff_pros", "dlg_done"});
}

void mmInterface::Reset()
{
    SetStateDefaults();

    // Initialize vehicle list if not already done (original game.asm calls LoadAll during startup)
    if (!VehicleListPtr)
    {
        VehicleListPtr = new mmVehList();
        VehicleListPtr->LoadAll();
    }
}

void mmInterface::ShowMain(i32 /*arg1*/)
{
    MenuMgr()->SetDefaultBackgroundImage("main_back");
    MenuMgr()->Switch(IDM_MAIN);
    MenuMgr()->SetFocus(MenuMain);
    MenuMgr()->AddPointer();
}

void mmInterface::Update()
{
    if (MenuMgr())
    {
        MenuMgr()->CheckInput();

        // Process pending menu actions (state_ == 4 means a widget was activated)
        if (UIMenu* menu = MenuMgr()->GetCurrentMenu())
        {
            if (menu->GetState() == 4)
            {
                UIBMButton::PlayClickSound();
                i32 widget_id = menu->GetWidgetID();
                menu->ClearAction();

                Displayf("Menu action: menu=%d widget=%d", menu->GetMenuID(), widget_id);

                // Main menu button dispatch
                if (menu->GetMenuID() == IDM_MAIN)
                {
                    switch (widget_id)
                    {
                        case IDC_MAIN_MENU_QUICK:
                            MenuMgr()->Switch(IDM_VEHICLE);
                            break;
                        case IDC_MAIN_MENU_SINGLE:
                            MenuMgr()->Switch(IDM_DRIVER);
                            break;
                        case IDC_MAIN_MENU_MULTI:
                            MenuMgr()->Switch(IDM_NET_SELECT);
                            break;
                        case IDC_MAIN_MENU_RECORDS:
                            // TODO: Show records dialog
                            break;
                        case IDC_MAIN_MENU_OPTIONS:
                            MenuMgr()->Switch(IDM_OPTIONS);
                            break;
                        case IDC_MAIN_MENU_HELP:
                            // TODO: Show help dialog
                            break;
                        case IDC_MAIN_MENU_MINIMIZE:
                            if (g_MainWindow)
                                SDL_MinimizeWindow(g_MainWindow);
                            break;
                        case IDC_MAIN_MENU_CLOSE:
                            MenuMgr()->Switch(IDD_QUIT);
                            break;
                    }
                }
                // Options menu dispatch
                else if (menu->GetMenuID() == IDM_OPTIONS)
                {
                    switch (widget_id)
                    {
                        case IDC_OPTIONS_MENU_AUDIO:
                            MenuMgr()->Switch(IDM_AUDIO);
                            break;
                        case IDC_OPTIONS_MENU_CONTROLS:
                            MenuMgr()->Switch(IDM_CONTROLS);
                            break;
                        case IDC_OPTIONS_MENU_GRAPHICS:
                            MenuMgr()->Switch(IDM_GRAPHICS);
                            break;
                        case IDC_OPTIONS_MENU_CREDITS:
                            MenuMgr()->Switch(IDM_ABOUT);
                            break;
                    }
                }
                // Driver menu dispatch
                else if (menu->GetMenuID() == IDM_DRIVER)
                {
                    switch (widget_id)
                    {
                        case IDC_DRIVER_NEW:
                        {
                            auto* driver = static_cast<DriverMenu*>(menu);
                            driver->NewPlayer();
                            MenuMgr()->Switch(IDM_DRIVER);
                            break;
                        }
                        case IDC_DRIVER_DELETE:
                        {
                            auto* driver = static_cast<DriverMenu*>(menu);
                            driver->DeleteCB();
                            break;
                        }
                        case IDC_DRIVER_STATS:
                            // TODO: Open driver records dialog (IDD_DREC)
                            break;
                        case IDC_DRIVER_PREV:
                        {
                            auto* driver = static_cast<DriverMenu*>(menu);
                            driver->DecPlayer();
                            break;
                        }
                        case IDC_DRIVER_NEXT:
                        {
                            auto* driver = static_cast<DriverMenu*>(menu);
                            driver->IncPlayer();
                            break;
                        }
                        case IDC_DRIVER_SELECT:
                            MenuMgr()->Switch(IDM_RACE);
                            break;
                    }
                }
                // Placeholder sub-option menus — Done button goes back to options
                else if (menu->GetMenuID() == IDM_AUDIO || menu->GetMenuID() == IDM_GRAPHICS ||
                         menu->GetMenuID() == IDM_CONTROLS || menu->GetMenuID() == IDM_ABOUT)
                {
                    if (widget_id == IDC_PLACEHOLDER_DONE)
                    {
                        // Preserve Options' previous menu ID (SwitchNow overwrites it)
                        if (UIMenu* options = MenuMgr()->GetMenu(IDM_OPTIONS))
                        {
                            i32 saved_prev = options->GetPreviousMenuID();
                            MenuMgr()->Switch(IDM_OPTIONS);
                            options->SetPreviousMenuID(saved_prev);
                        }
                        else
                        {
                            MenuMgr()->Switch(IDM_OPTIONS);
                        }
                    }
                }
                // Vehicle menu
                else if (menu->GetMenuID() == IDM_VEHICLE)
                {
                    switch (widget_id)
                    {
                        case IDC_VEHICLE_BACK:
                            MenuMgr()->Switch(IDM_MAIN);
                            break;
                        case IDC_VEHICLE_DRIVE:
                            // TODO: Start race
                            break;
                        case IDC_VEHICLE_SELECT:
                            MenuMgr()->Switch(IDM_SHOWCASE);
                            break;
                        case IDC_VEHICLE_AUTO:
                            // TODO: Toggle transmission
                            break;
                        case IDC_VEHICLE_PREV:
                        {
                            if (VehicleSelectBase* vs = static_cast<VehicleSelectBase*>(menu))
                                vs->DecCar();
                            break;
                        }
                        case IDC_VEHICLE_NEXT:
                        {
                            if (VehicleSelectBase* vs = static_cast<VehicleSelectBase*>(menu))
                                vs->IncCar();
                            break;
                        }
                    }
                }
                // Quit confirmation dialog
                else if (menu->GetMenuID() == IDD_QUIT)
                {
                    if (widget_id == IDC_QUIT_YES)
                    {
                        if (CloseCallback)
                            CloseCallback();
                    }
                    else if (widget_id == IDC_QUIT_NO)
                    {
                        if (i32 prev = MenuMgr()->GetPreviousMenu(); prev >= 0)
                            MenuMgr()->Switch(prev);
                        else
                            MenuMgr()->Switch(IDM_MAIN);
                    }
                }
                // VehShowcase — back/any key returns to vehicle selection
                else if (menu->GetMenuID() == IDM_SHOWCASE)
                {
                    MenuMgr()->Switch(IDM_VEHICLE);
                }
            }
        }

        // Process nav bar action
        if (uiNavBar* nav = static_cast<uiNavBar*>(MenuMgr()->GetNavBar()))
        {
            if (nav->GetState() == 4)
            {
                UIBMButton::PlayClickSound();
                i32 widget_id = nav->GetWidgetID();
                nav->ClearAction();

                Displayf("Nav action: widget=%d", widget_id);

                switch (widget_id)
                {
                    case IDC_NAV_OPT:
                        MenuMgr()->Switch(IDM_OPTIONS);
                        break;
                    case IDC_NAV_HELP:
                        break;
                    case IDC_NAV_STOW:
                        if (g_MainWindow)
                            SDL_MinimizeWindow(g_MainWindow);
                        break;
                    case IDC_NAV_EXIT:
                    {
                        MenuMgr()->Switch(IDD_QUIT);
                        break;
                    }
                    case IDC_NAV_PREV:
                        if (i32 prev = MenuMgr()->GetPreviousMenu(); prev >= 0)
                            MenuMgr()->Switch(prev);
                        break;
                }
            }
        }
    }

    asNode::Update();
}