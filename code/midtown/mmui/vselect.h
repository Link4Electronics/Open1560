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

    i32 CurrentCar() const { return *reinterpret_cast<const i32*>(&gap90[0x00]); }
    i32 CurrentColor() const { return *reinterpret_cast<const i32*>(&gap90[0x04]); }
    i32 VehicleCount() const { return *reinterpret_cast<const i32*>(&gap90[0x08]); }

    void SetCurrentCar(i32 v) { *reinterpret_cast<i32*>(&gap90[0x00]) = v; }
    void SetCurrentColor(i32 v) { *reinterpret_cast<i32*>(&gap90[0x04]) = v; }

    asDofCS* GetDofCSArray() const { return *reinterpret_cast<asDofCS* const*>(&gap90[0x3C]); }
    void SetDofCSArray(asDofCS* v) { *reinterpret_cast<asDofCS**>(&gap90[0x3C]) = v; }

    mmVehicleForm* GetVehicleFormArray() const { return *reinterpret_cast<mmVehicleForm* const*>(&gap90[0x4C]); }
    void SetVehicleFormArray(mmVehicleForm* v) { *reinterpret_cast<mmVehicleForm**>(&gap90[0x4C]) = v; }

    i32* GetTopSpeedArray() const { return *reinterpret_cast<i32* const*>(&gap90[0x50]); }
    void SetTopSpeedArray(i32* v) { *reinterpret_cast<i32**>(&gap90[0x50]) = v; }

    i32 GetUnlockLevel() const { return *reinterpret_cast<const i32*>(&gap90[0xD4]); }
    void SetUnlockLevel(i32 v) { *reinterpret_cast<i32*>(&gap90[0xD4]) = v; }

protected:
    u8 gap90[0xD8];
};

check_size(VehicleSelectBase, 0x168);

check_size(VehicleSelectBase, 0x168);
