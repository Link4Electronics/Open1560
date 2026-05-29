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

define_dummy_symbol(mmwidget_navbar);

#include "navbar.h"

#include "bm_button.h"
#include "manager.h"

#include "pcwindis/dxinit.h"

#include <SDL3/SDL_video.h>

uiNavBar::uiNavBar()
    : UIMenu(0)
{
    menu_x_ = 0.0f;
    menu_y_ = 0.0f;
    menu_width_ = 1.0f;
    menu_height_ = 1.0f;

    AssignName(LOC_TEXT("Nav Bar"));

    mnav_opt_btn_ = AddBMButton(IDC_NAV_OPT, "mnav_opt"_xconst, 0.72f, 0.0f, 5);
    mnav_opt_btn_->Height = 0.05f;
    mnav_opt_btn_->MaxY = 0.05f;

    {
        UIBMButton* help = AddBMButton(IDC_NAV_HELP, "mnav_help"_xconst, 0.80f, 0.0f, 3);
        help->Height = 0.05f;
        help->MaxY = 0.05f;
    }
    {
        UIBMButton* stow = AddBMButton(IDC_NAV_STOW, "mnav_stow"_xconst, 0.88f, 0.0f, 3);
        stow->Height = 0.05f;
        stow->MaxY = 0.05f;
    }
    {
        UIBMButton* exit = AddBMButton(IDC_NAV_EXIT, "mnav_exit"_xconst, 0.96f, 0.0f, 3);
        exit->Height = 0.05f;
        exit->MaxY = 0.05f;
    }

    mnav_prev_btn_ = AddBMButton(IDC_NAV_PREV, "mnav_prev"_xconst, 0.0f, 0.9f, 5);
    mnav_prev_btn_->Height = 0.05f;
    mnav_prev_btn_->MaxY = 0.95f;

    previous_x_ = mnav_prev_btn_->X;
    previous_y_ = mnav_prev_btn_->Y;

    ActivateNode();
    enabled_ = true;
    SetBstate(0);
}

void uiNavBar::BackUp()
{
    state_ = MENU_STATE_1;
}

void uiNavBar::Help()
{
    // TODO: implement help dialog
}

void uiNavBar::Minimize()
{
    if (g_MainWindow)
        SDL_MinimizeWindow(g_MainWindow);
}

void uiNavBar::OptionActive()
{
    mnav_opt_btn_->MexOn();
    option_active_ = 1;
}

void uiNavBar::OptionInActive()
{
    mnav_opt_btn_->MexOff();
    option_active_ = 0;
}

void uiNavBar::SetPrevPos(f32 x, f32 y)
{
    if (x < -1.0f)
    {
        mnav_prev_btn_->SetPosition(previous_x_, previous_y_);
    }
    else
    {
        mnav_prev_btn_->SetPosition(x, y);
    }
}

void uiNavBar::TurnOffPrev()
{
    prev_off_ = 1;
}

void uiNavBar::TurnOnPrev()
{
    prev_off_ = 0;
}

void uiNavBar::Update()
{
    // Show previous button when there's a menu to go back to
    if (!prev_off_ && MenuMgr()->GetPreviousMenu() >= 0)
        mnav_prev_btn_->Enable();
    else
        mnav_prev_btn_->Disable();

    UIMenu::Update();
}
