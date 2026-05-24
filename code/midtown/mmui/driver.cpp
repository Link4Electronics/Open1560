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

#include "agi/pipeline.h"
#include "localize/localize.h"

DriverMenu::DriverMenu(i32 arg1)
    : UIMenu(arg1)
{
    SetBstate(0);
}

DriverMenu::~DriverMenu() = default;

void DriverMenu::AddPlayer(char* /*arg1*/)
{}

void DriverMenu::DeleteCB()
{}

void DriverMenu::DisplayDriverInfo(char* /*arg1*/, char* /*arg2*/, char* /*arg3*/, char* /*arg4*/, char* /*arg5*/,
    i32 /*arg6*/)
{}

void DriverMenu::InitPlayerSelection()
{}

void DriverMenu::NewPlayer()
{}

void DriverMenu::PreSetup()
{
    UIMenu::PreSetup();
}

void DriverMenu::RemoveAllPlayers()
{}

void DriverMenu::RemovePlayer(char* /*arg1*/)
{}

void DriverMenu::SetController(char* /*arg1*/)
{}

void DriverMenu::SetNetName(char* /*arg1*/)
{}

void DriverMenu::SetPlayerPick(i32 /*arg1*/)
{}

void DriverMenu::TDPickCB()
{}
