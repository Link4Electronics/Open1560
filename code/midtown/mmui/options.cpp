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

define_dummy_symbol(mmui_options);

#include "options.h"

#include "midtown.h"
#include "mmwidget/manager.h"
#include "mmwidget/navbar.h"

OptionsMenu::OptionsMenu(i32 menu_id)
    : UIMenu(menu_id)
{
    AssignName(LOC_TEXT("Main Options Menu"));
    AssignBackground("omain_back");

    AddBMButton(IDC_OPTIONS_MENU_AUDIO, "onav_aud"_xconst, UI_LEFT_MARGIN, 0.2f, 4);
    AddBMButton(IDC_OPTIONS_MENU_CONTROLS, "onav_cont"_xconst, UI_LEFT_MARGIN, 0.4f, 4);
    AddBMButton(IDC_OPTIONS_MENU_GRAPHICS, "onav_gra"_xconst, UI_LEFT_MARGIN, 0.6f, 4);
    AddBMButton(IDC_OPTIONS_MENU_CREDITS, "onav_credit"_xconst, UI_LEFT_MARGIN, 0.8f, 4);

    uiWidget* hotspot = AddHotSpot(-1, "Previous"_xconst, 0.9f, 0.9f, 0.9f, 0.0f);
    previous_x_ = hotspot->X;
    previous_y_ = hotspot->Y;

    SetBstate(0);
}

void OptionsMenu::PostSetup()
{
    MenuMgr()->GetNavBar()->SetPrevPos(0.0f, 0.0f);
    MenuMgr()->GetNavBar()->OptionInActive();
}

void OptionsMenu::PreSetup()
{
    if (!GraphicsChange)
        GraphicsPreviousMenu = static_cast<u8>(prev_menu_id_);

    MenuMgr()->GetNavBar()->SetPrevPos(previous_x_, previous_y_);
    MenuMgr()->GetNavBar()->OptionActive();
}
