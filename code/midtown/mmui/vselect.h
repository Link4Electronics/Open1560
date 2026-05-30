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

class asDofCS;
class Card2D;
class UIBMLabel;
class UISlider;
class UITextDropdown;
class mmVehicleForm;

class VehicleSelectBase : public UIMenu
{
public:
    ARTS_IMPORT VehicleSelectBase(i32 arg1);
    ARTS_IMPORT ~VehicleSelectBase() override;

    ARTS_IMPORT void AllSetCar(char* arg1, i32 arg2);
    ARTS_IMPORT void AssignVehicleStats(i32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5);
    ARTS_IMPORT void CarMod(i32& arg1);
    ARTS_IMPORT void ColorCB();
    ARTS_IMPORT i32 CurrentVehicleIsLocked();
    ARTS_IMPORT void DecCar();
    ARTS_IMPORT void DecColor();
    ARTS_IMPORT void FillStats();
    ARTS_IMPORT char* GetCarTitle(i32 arg1, char* arg2, i16 arg3, string* arg4);
    ARTS_IMPORT void IncCar();
    ARTS_IMPORT void IncColor();
    ARTS_IMPORT void InitCarSelection(i32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5);
    ARTS_IMPORT i32 LoadStats(char* arg1);
    ARTS_IMPORT void PostSetup() override;
    ARTS_IMPORT void PreSetup() override;
    ARTS_EXPORT void Reset() override;
    ARTS_IMPORT void SetLastUnlockedVehicle();
    ARTS_IMPORT void SetLockedLabel();
    ARTS_IMPORT void SetPick(i32 arg1, i16 arg2);
    ARTS_IMPORT void SetShowcaseFlag();
    ARTS_IMPORT void TDPickCB();
    ARTS_IMPORT void Update() override;

    i32 CurrentCar() const { return current_car_; }
    i32 CurrentColor() const { return current_color_; }
    i32 VehicleCount() const { return vehicle_count_; }

    void SetCurrentCar(i32 v) { current_car_ = v; }
    void SetCurrentColor(i32 v) { current_color_ = v; }

    asDofCS* GetDofCSArray() const { return dofcs_array_; }
    void SetDofCSArray(asDofCS* v) { dofcs_array_ = v; }

    mmVehicleForm* GetVehicleFormArray() const { return forms_array_; }
    void SetVehicleFormArray(mmVehicleForm* v) { forms_array_ = v; }

    i32* GetTopSpeedArray() const { return top_speed_array_; }
    void SetTopSpeedArray(i32* v) { top_speed_array_ = v; }

    i32* GetExtraArray() const { return extra_array_; }
    void SetExtraArray(i32* v) { extra_array_ = v; }

    i32 GetUnlockLevel() const { return unlock_level_; }
    void SetUnlockLevel(i32 v) { unlock_level_ = v; }

protected:
    // gap90 stores i32/f32 fields whose positions are tied to the original 32-bit layout.
    // Pointer-size fields (dofCS, forms, topSpeed, extraArray) are stored as proper member
    // variables below to avoid overlaps when 32-bit 4-byte pointer slots become 8 bytes on 64-bit.
    u8 gap90[0xD8];

    i32 current_car_ {};
    i32 current_color_ {};
    i32 vehicle_count_ {};

    asDofCS* dofcs_array_ {};
    mmVehicleForm* forms_array_ {};
    i32* top_speed_array_ {};
    i32* extra_array_ {};
    i32 unlock_level_ {};
};

check_size(VehicleSelectBase, 0x168);

check_size(VehicleSelectBase, 0x168);
