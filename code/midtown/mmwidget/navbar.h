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

#pragma once

#include "menu.h"

class UIBMButton;

enum
{
    IDC_NAV_OPT = 0x64,
    IDC_NAV_STOW = 0x65,
    IDC_NAV_HELP = 0x66,
    IDC_NAV_EXIT = 0x67,
    IDC_NAV_PREV = 0x68,
};

class uiNavBar final : public UIMenu
{
public:
    uiNavBar();

    ~uiNavBar() override = default;

    void BackUp() override;

    void Help();
    void Minimize();
    void OptionActive();
    void OptionInActive();
    void SetPrevPos(f32 x, f32 y);
    void TurnOffPrev();
    void TurnOnPrev();
    void Update() override;

    UIBMButton* mnav_opt_btn_ {nullptr};
    UIBMButton* mnav_prev_btn_ {nullptr};
    f32 previous_x_ {0.0f};
    f32 previous_y_ {0.0f};
    b32 prev_off_ {0};
    b32 option_active_ {0};
};
