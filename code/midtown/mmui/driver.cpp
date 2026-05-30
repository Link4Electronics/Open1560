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

define_dummy_symbol(mmui_driver);

#include "driver.h"

#include "agi/bitmap.h"
#include "agi/pipeline.h"
#include "arts7/cullmgr.h"
#include "arts7/sim.h"
#include "localize/localize.h"
#include "mmcityinfo/playerdata.h"
#include "mmcityinfo/state.h"
#include "mmwidget/manager.h"
#include "mmwidget/navbar.h"
#include "mmwidget/textdrop.h"
#include "mmeffects/mmtext.h"
#include "stream/stream.h"

#include <cstdio>
#include <cstring>
#include <sys/stat.h>

static constexpr i32 MAX_PLAYERS = 18;
static constexpr i32 NAME_LEN = 40;

struct PlayerList
{
    i32 Count = 0;
    char Names[MAX_PLAYERS][NAME_LEN] {};
    char Files[MAX_PLAYERS][NAME_LEN] {};
};

static PlayerList& GetPlayerList()
{
    static PlayerList list;
    return list;
}

static void SavePlayers()
{
    auto& list = GetPlayerList();

    // Ensure players directory exists
    mkdir("players", 0755);

    Ptr<Stream> file {arts_fopen("players/players.dir", "w")};
    if (!file)
        return;

    arts_fprintf(file.get(), "%d\n", list.Count);

    for (i32 i = 0; i < list.Count; ++i)
    {
        arts_fprintf(file.get(), "%s %s\n", list.Names[i], list.Files[i]);
    }
}

static void LoadPlayers()
{
    auto& list = GetPlayerList();
    list.Count = 0;

    bool fresh = false;

    Ptr<Stream> file {arts_fopen("players/players.dir", "r")};
    if (!file)
    {
        fresh = true;
    }
    else
    {
        char line[256];

        if (file->Gets(line, sizeof(line)) <= 0)
        {
            fresh = true;
        }
        else
        {
            i32 count = 0;
            std::sscanf(line, "%d", &count);

            if (count > MAX_PLAYERS)
                count = MAX_PLAYERS;

            for (i32 i = 0; i < count; ++i)
            {
                if (file->Gets(line, sizeof(line)) <= 0)
                    break;

                char name[NAME_LEN] {};
                char fname[NAME_LEN] {};
                std::sscanf(line, "%39s %39s", name, fname);

                if (name[0])
                {
                    arts_strcpy(list.Names[list.Count], name);
                    arts_strcpy(list.Files[list.Count], fname[0] ? fname : name);
                    ++list.Count;
                }
            }
        }
    }

    if (list.Count == 0)
    {
        arts_strcpy(list.Names[0], "DriverX");
        arts_strcpy(list.Files[0], "player0");
        list.Count = 1;
        fresh = true;
    }

    if (fresh)
        SavePlayers();
}

DriverMenu::DriverMenu(i32 menu_id)
    : UIMenu(menu_id)
{
    AssignName(LOC_TEXT("Driver Select Menu"));
    AssignBackground("driv_back");

    InitPlayerSelection();

    SetBstate(0);
}

void DriverMenu::InitPlayerSelection()
{
    // Compute dropdown position using driver menu fields (matching game.asm layout:
    // dropdown_x = 0.114, dropdown_y = 0.05, dropdown_w = 0.375, dropdown_h = 0.0667)
    f32 dd_x = UI_LEFT_MARGIN;
    f32 dd_y = 0.05f;
    f32 dd_w = 0.37f;
    f32 dd_h = 0.0667f;

    // Build initial options string (will be updated in PreSetup)
    string options = "DriverX";

    // Create dropdown for player selection
    player_dd_ = AddTextDropdown(-1, LOC_TEXT("DRIVER"), &current_player_,
        dd_x, dd_y, dd_w, dd_h, std::move(options), 18, 1, 0, Callback(), nullptr);

    if (player_dd_)
    {
        // Position the value text to show after the label
        // (the dropdown's value text node is automatically created)
    }

    // Create text node for driver info display (below dropdown)
    Ptr<mmTextNode> text_node = arnew mmTextNode();
    text_node->Init(UI_LEFT_MARGIN, dd_y + dd_h + 0.02f, 0.7f, 0.55f, 12, BITMAP_TRANSPARENT);

    void* font16 = MenuMgr()->GetFont(16);

    // Interleave label/value pairs so odd lines are values, even lines are labels
    // Line 0: "RANKING:" label
    text_node->AddText(font16, LOC_STRING(MM_IDS_365), MM_TEXT_REQUIRED, 0.0f, 0.0f);
    // Line 1: Ranking value (dynamic)
    text_node->AddText(font16, LOC_TEXT(""), 0, 0.3f, 0.0f);

    // Line 2: "LAST RACE:" label
    text_node->AddText(font16, LOC_STRING(MM_IDS_366), MM_TEXT_REQUIRED, 0.0f, 0.06f);
    // Line 3: Last race value (dynamic)
    text_node->AddText(font16, LOC_TEXT("None"), 0, 0.3f, 0.06f);

    // Line 4: "LAST VEHICLE:" label
    text_node->AddText(font16, LOC_STRING(MM_IDS_367), MM_TEXT_REQUIRED, 0.0f, 0.12f);
    // Line 5: Last vehicle value (dynamic)
    text_node->AddText(font16, LOC_TEXT("None"), 0, 0.3f, 0.12f);

    // Line 6: "CONTROLLER:" label
    text_node->AddText(font16, LOC_STRING(MM_IDS_368), MM_TEXT_REQUIRED, 0.0f, 0.18f);
    // Line 7: Controller value (dynamic)
    text_node->AddText(font16, LOC_TEXT("Keyboard/Mouse"), 0, 0.3f, 0.18f);

    // Line 8: "NETNAME:" label
    text_node->AddText(font16, LOC_STRING(MM_IDS_369), MM_TEXT_REQUIRED, 0.0f, 0.24f);
    // Line 9: Net name value (dynamic)
    text_node->AddText(font16, LOC_TEXT("Local"), 0, 0.3f, 0.24f);

    // Line 10: "SCORE:" label
    text_node->AddText(font16, LOC_STRING(MM_IDS_370), MM_TEXT_REQUIRED, 0.0f, 0.30f);
    // Line 11: Score value (dynamic)
    text_node->AddText(font16, LOC_TEXT("0"), 0, 0.3f, 0.30f);

    info_text_ = text_node.get();
    AdoptChild(Ptr<asNode>(std::move(text_node)));

    // Prev/Next buttons for player selection (above the info panel)
    AddBMButton(IDC_DRIVER_PREV, "main_prev"_xconst, dd_x + dd_w + 0.01f, dd_y, 4);
    AddBMButton(IDC_DRIVER_NEXT, "main_next"_xconst, dd_x + dd_w + 0.10f, dd_y, 4);

    // Bottom action buttons
    f32 btn_y = 0.72f;
    AddBMButton(IDC_DRIVER_NEW, "driv_new"_xconst, UI_LEFT_MARGIN, btn_y, 4);
    AddBMButton(IDC_DRIVER_DELETE, "driv_del"_xconst, UI_LEFT_MARGIN + 0.22f, btn_y, 4);
    AddBMButton(IDC_DRIVER_STATS, "driv_stats"_xconst, UI_LEFT_MARGIN + 0.44f, btn_y, 4);
    AddBMButton(IDC_DRIVER_SELECT, "driv_next"_xconst, UI_LEFT_MARGIN + 0.66f, btn_y, 4);
}

void DriverMenu::PreSetup()
{
    UIMenu::PreSetup();

    LoadPlayers();

    // Build options string from player list
    {
        auto& list = GetPlayerList();
        string options;

        for (i32 i = 0; i < list.Count; ++i)
        {
            if (i > 0)
                options += "|";
            options += list.Names[i];
        }

        if (player_dd_)
        {
            player_dd_->AssignString(std::move(options));
            player_dd_->SetValue(current_player_ < 0 ? 0 : current_player_);
        }
    }

    if (current_player_ < 0)
        current_player_ = 0;

    SetPlayerPick(current_player_);

    DisplayDriverInfo(nullptr, nullptr, nullptr, controller_name_, net_name_, MMCURRPLAYER.GetTotalScore());
}

void DriverMenu::PostSetup()
{
    MenuMgr()->GetNavBar()->SetPrevPos(0.0f, 0.0f);
}

void DriverMenu::SetPlayerPick(i32 index)
{
    auto& list = GetPlayerList();

    if (index < 0 || index >= list.Count)
        return;

    current_player_ = index;

    char player_path[128];
    arts_snprintf(player_path, sizeof(player_path), "players/%s", list.Files[index]);

    MMCURRPLAYER.Reset();
    arts_strcpy(MMCURRPLAYER.PlayerName, list.Names[index]);
    arts_strcpy(MMCURRPLAYER.FileName, list.Files[index]);
    arts_strcpy(MMCURRPLAYER.NetName, list.Names[index]);
    MMCURRPLAYER.Load(player_path);
}

void DriverMenu::TDPickCB()
{
    SetPlayerPick(current_player_);
    DisplayDriverInfo(nullptr, nullptr, nullptr, controller_name_, net_name_, MMCURRPLAYER.GetTotalScore());
}

void DriverMenu::IncPlayer()
{
    auto& list = GetPlayerList();
    if (list.Count <= 1)
        return;

    current_player_ = (current_player_ + 1) % list.Count;

    if (player_dd_)
        player_dd_->SetValue(current_player_);

    SetPlayerPick(current_player_);
    DisplayDriverInfo(nullptr, nullptr, nullptr, controller_name_, net_name_, MMCURRPLAYER.GetTotalScore());
}

void DriverMenu::DecPlayer()
{
    auto& list = GetPlayerList();
    if (list.Count <= 1)
        return;

    current_player_ = (current_player_ - 1 + list.Count) % list.Count;

    if (player_dd_)
        player_dd_->SetValue(current_player_);

    SetPlayerPick(current_player_);
    DisplayDriverInfo(nullptr, nullptr, nullptr, controller_name_, net_name_, MMCURRPLAYER.GetTotalScore());
}

void DriverMenu::DisplayDriverInfo(
    char* /*ranking*/, char* last_race, char* last_vehicle, char* controller, char* netname, i32 score)
{
    if (!info_text_)
        return;

    // Line 1: ranking (unused for now)

    // Line 3: Last race
    const char* race = "None";
    if (MMCURRPLAYER.LastGamePicked >= 0 && MMCURRPLAYER.LastRacePicked >= 0)
    {
        race = last_race ? last_race : "None";
    }
    info_text_->SetString(3, LOC_TEXT(race));

    // Line 5: Last vehicle
    const char* vehicle = MMCURRPLAYER.LastCarPicked;
    if (!vehicle || !vehicle[0])
        vehicle = "None";
    info_text_->SetString(5, LOC_TEXT(vehicle));

    // Line 7: Controller
    const char* ctrl = controller ? controller : "Keyboard/Mouse";
    info_text_->SetString(7, LOC_TEXT(ctrl));

    // Line 9: Net name
    const char* net = netname ? netname : "Local";
    info_text_->SetString(9, LOC_TEXT(net));

    // Line 11: Score
    char score_str[32];
    arts_snprintf(score_str, sizeof(score_str), "%d", score);
    info_text_->SetString(11, LOC_TEXT(score_str));
}

void DriverMenu::AddPlayer(char* name)
{
    auto& list = GetPlayerList();

    if (list.Count >= MAX_PLAYERS)
        return;

    arts_strcpy(list.Names[list.Count], name);
    arts_snprintf(list.Files[list.Count], NAME_LEN, "player%d", list.Count);

    // Create a blank .sav file for the new player
    char sav_path[128];
    arts_snprintf(sav_path, sizeof(sav_path), "players/%s.sav", list.Files[list.Count]);
    Ptr<Stream> sav {arts_fopen(sav_path, "w")};
    if (sav)
    {
        arts_fprintf(sav.get(), "Name=\"%s\"\n", name);
        arts_fprintf(sav.get(), "NetName=\"%s\"\n", name);
    }

    ++list.Count;

    SavePlayers();
}

void DriverMenu::RemovePlayer(char* name)
{
    auto& list = GetPlayerList();

    i32 found = -1;
    for (i32 i = 0; i < list.Count; ++i)
    {
        if (!std::strcmp(list.Names[i], name))
        {
            found = i;
            break;
        }
    }

    if (found < 0)
        return;

    for (i32 i = found; i < list.Count - 1; ++i)
    {
        arts_strcpy(list.Names[i], list.Names[i + 1]);
        arts_strcpy(list.Files[i], list.Files[i + 1]);
    }

    --list.Count;

    SavePlayers();
}

void DriverMenu::RemoveAllPlayers()
{
    auto& list = GetPlayerList();
    list.Count = 0;
    SavePlayers();
}

void DriverMenu::SetController(char* name)
{
    if (name)
        arts_strcpy(controller_name_, name);
}

void DriverMenu::SetNetName(char* name)
{
    if (name)
        arts_strcpy(net_name_, name);
}

void DriverMenu::NewPlayer()
{
    auto& list = GetPlayerList();
    char name[NAME_LEN];
    arts_snprintf(name, sizeof(name), "Driver%d", list.Count + 1);
    AddPlayer(name);
}

void DriverMenu::DeleteCB()
{
    auto& list = GetPlayerList();
    if (list.Count <= 1)
        return;

    if (current_player_ >= 0 && current_player_ < list.Count)
    {
        RemovePlayer(list.Names[current_player_]);

        if (current_player_ >= list.Count)
            current_player_ = list.Count - 1;

        SetPlayerPick(current_player_);
    }
}
