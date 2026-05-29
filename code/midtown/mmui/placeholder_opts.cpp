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

define_dummy_symbol(mmui_placeholder_opts);

#include "placeholder_opts.h"

#include "mmwidget/manager.h"
#include "mmwidget/navbar.h"

// ── AudioOptionMenu ──────────────────────────────────────────────

AudioOptionMenu::AudioOptionMenu(i32 menu_id)
    : UIMenu(menu_id)
{
    AssignName(LOC_TEXT("Audio Options"));
    AssignBackground("oaud_back");

    AddBMButton(IDC_PLACEHOLDER_DONE, "onav_done"_xconst, UI_LEFT_MARGIN, 0.8f, 4);

    SetBstate(0);
}

void AudioOptionMenu::PreSetup()
{
    prev_menu_id_ = IDM_OPTIONS;
}

void AudioOptionMenu::PostSetup()
{
    MenuMgr()->GetNavBar()->SetPrevPos(0.0f, 0.0f);
}

// ── GraphicsOptionMenu ───────────────────────────────────────────

GraphicsOptionMenu::GraphicsOptionMenu(i32 menu_id)
    : UIMenu(menu_id)
{
    AssignName(LOC_TEXT("Graphics Options"));
    AssignBackground("ogra_back");

    AddBMButton(IDC_PLACEHOLDER_DONE, "onav_done"_xconst, UI_LEFT_MARGIN, 0.8f, 4);

    SetBstate(0);
}

void GraphicsOptionMenu::PreSetup()
{
    prev_menu_id_ = IDM_OPTIONS;
}

void GraphicsOptionMenu::PostSetup()
{
    MenuMgr()->GetNavBar()->SetPrevPos(0.0f, 0.0f);
}

// ── ControlOptionMenu ────────────────────────────────────────────

ControlOptionMenu::ControlOptionMenu(i32 menu_id)
    : UIMenu(menu_id)
{
    AssignName(LOC_TEXT("Controls Options"));
    AssignBackground("ocon_back");

    AddBMButton(IDC_PLACEHOLDER_DONE, "onav_done"_xconst, UI_LEFT_MARGIN, 0.8f, 4);

    SetBstate(0);
}

void ControlOptionMenu::PreSetup()
{
    prev_menu_id_ = IDM_OPTIONS;
}

void ControlOptionMenu::PostSetup()
{
    MenuMgr()->GetNavBar()->SetPrevPos(0.0f, 0.0f);
}

// ── AboutOptionMenu ──────────────────────────────────────────────

AboutOptionMenu::AboutOptionMenu(i32 menu_id)
    : UIMenu(menu_id)
{
    AssignName(LOC_TEXT("About"));
    AssignBackground("about_back");

    AddBMButton(IDC_PLACEHOLDER_DONE, "onav_done"_xconst, UI_LEFT_MARGIN, 0.8f, 4);

    SetBstate(0);
}

void AboutOptionMenu::PreSetup()
{
    prev_menu_id_ = IDM_OPTIONS;
}

void AboutOptionMenu::PostSetup()
{
    MenuMgr()->GetNavBar()->SetPrevPos(0.0f, 0.0f);
}
