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

define_dummy_symbol(mmui_vehicle);

#include "vehicle.h"

#include "mmwidget/manager.h"

// Viewport is at screen (0.053, 0.365, 0.453, 0.45) — left mid area
// Buttons must be placed RIGHT of viewport (screen x > 0.506) or BELOW (screen y > 0.815)
// ScaleWidget: screen = menu(0.114,0.07) + widget * menu(0.775,0.855)
// Right column widget x > (0.506-0.114)/0.775 = 0.506
// Bottom row  widget y > (0.815-0.07)/0.855 = 0.871
static constexpr f32 VB_X_RIGHT = 0.55f;     // right column (screen ~0.540)
static constexpr f32 VB_X_LEFT = 0.10f;      // bottom left (screen ~0.192)
static constexpr f32 VB_Y_TOP = 0.05f;       // above viewport (screen ~0.113)
static constexpr f32 VB_Y_MID = 0.35f;       // right of viewport top (screen ~0.369)
static constexpr f32 VB_Y_BOTTOM = 0.88f;    // below viewport (screen ~0.822)

Vehicle::Vehicle(i32 arg1)
    : VehicleSelectBase(arg1)
{
    AddBMButton(IDC_VEHICLE_AUTO,    "veh_auto"_xconst,  VB_X_RIGHT, VB_Y_TOP,    4);
    AddBMButton(IDC_VEHICLE_SELECT,  "vehi_show"_xconst, VB_X_RIGHT, VB_Y_MID,    4);
    AddBMButton(IDC_VEHICLE_BACK,    "onav_done"_xconst, VB_X_LEFT,  VB_Y_BOTTOM, 4);
    AddBMButton(IDC_VEHICLE_DRIVE,   "vehi_play"_xconst, VB_X_RIGHT, VB_Y_BOTTOM, 4);
}

void Vehicle::PreSetup()
{
    AssignBackground("veh_back");

    // Initialize car selection with viewport parameters if not yet done
    // Viewport: left-center area of the menu
    InitCarSelection(0, 0.05f, 0.35f, 0.50f, 0.50f);

    VehicleSelectBase::PreSetup();
}

void Vehicle::PostSetup()
{
    VehicleSelectBase::PostSetup();
}

void Vehicle::SetSubMenu(i32 /*arg1*/)
{}

void Vehicle::SetSubMenuButtons()
{}
