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

#include "mmwidget/menu.h"

class mmTextNode;

enum
{
    IDC_DRIVER_NEW = 50,
    IDC_DRIVER_DELETE = 51,
    IDC_DRIVER_STATS = 52,
    IDC_DRIVER_PREV = 53,
    IDC_DRIVER_NEXT = 54,
    IDC_DRIVER_SELECT = 9999,
};

class DriverMenu final : public UIMenu
{
public:
    DriverMenu(i32 menu_id);
    ~DriverMenu() override = default;

    void AddPlayer(char* name);
    void DeleteCB();
    void DecPlayer();
    void DisplayDriverInfo(char* ranking, char* last_race, char* last_vehicle, char* controller, char* netname, i32 score);
    void IncPlayer();
    void InitPlayerSelection();
    void NewPlayer();
    void PreSetup() override;
    void PostSetup() override;
    void RemoveAllPlayers();
    void RemovePlayer(char* name);
    void SetController(char* name);
    void SetNetName(char* name);
    void SetPlayerPick(i32 index);
    void TDPickCB();

private:
    char default_name_[40] {"DriverX"};
    i32 current_player_ {-1};
    char controller_name_[80] {};
    char net_name_[80] {};
    mmTextNode* info_text_ {nullptr};
};
