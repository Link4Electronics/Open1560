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

define_dummy_symbol(mmcityinfo_vehlist);

#include "vehlist.h"

#include "agiworld/texsheet.h"
#include "stream/fsystem.h"

mmVehList::mmVehList()
{
    VehicleListPtr = this;
}

mmVehList::~mmVehList()
{
    for (i32 i = 0; i < NumVehicles; ++i)
        delete Vehicles[i];

    VehicleListPtr = nullptr;
}

i32 mmVehList::GetVehicleID(aconst char* name)
{
    for (i32 i = 0; i < NumVehicles; ++i)
    {
        if (!std::strcmp(name, Vehicles[i]->BaseName))
            return i;
    }

    return -1;
}

mmVehInfo* mmVehList::GetVehicleInfo(const char* name)
{
    for (i32 i = 0; i < NumVehicles; ++i)
    {
        if (mmVehInfo* vehicle = Vehicles[i]; std::strcmp(vehicle->BaseName, name) == 0)
            return vehicle;
    }

    return DefaultVehicle;
}

mmVehInfo* mmVehList::GetVehicleInfo(i32 index)
{
    if (index >= 0 && index < NumVehicles)
        return Vehicles[index];

    Errorf("mmVehList::GetVehicleInfo Illegal id(%d)", index);

    return nullptr;
}

void mmVehList::Init(i32 /*arg1*/)
{
    NumVehicles = 0;
}

void mmVehList::Load(const char* name)
{
    Ptr<mmVehInfo> info = arnew mmVehInfo();

    if (!info->Load(arts_formatf<64>("tune/%s", name)) || GetVehicleID(info->BaseName) >= 0)
        return;

    Ptr<mmVehInfo*[]> vehicles = arnewa mmVehInfo*[NumVehicles + 1];

    for (i32 i = 0; i < NumVehicles; ++i)
        vehicles[i] = Vehicles[i];

    vehicles[NumVehicles] = info.release();
    Vehicles.swap(vehicles);
    ++NumVehicles;
}

void mmVehList::LoadAll()
{
    InitTexSheet();

    for (i32 i = 0; i < FileSystem::FSCount; ++i)
    {
        FileSystem* fs = FileSystem::FS[i];

        for (FileInfo* f = fs->FirstEntry("tune"); f; f = fs->NextEntry(f))
        {
            if (const char* ext = std::strrchr(f->Path, '.'); ext && !arts_stricmp(ext, ".INFO"))
                Load(f->Path);
        }
    }

    // If no .INFO files found, generate default vehicle info by scanning BMS directory
    if (NumVehicles == 0)
    {
        for (i32 i = 0; i < FileSystem::FSCount; ++i)
        {
            FileSystem* fs = FileSystem::FS[i];

            for (FileInfo* f = fs->FirstEntry("bms"); f; f = fs->NextEntry(f))
            {
                if (!f->IsDirectory)
                    continue;

                // Verify this is actually a vehicle by checking for BODY_H.BMS
                char test_path[256];
                arts_sprintf(test_path, "bms/%s/BODY_H.bms", f->Path);

                if (!fs->QueryOn(test_path))
                    continue;

                Ptr<mmVehInfo> info = arnew mmVehInfo();

                // Store BaseName in lowercase for compatibility
                char* dst = info->BaseName;
                const char* src = f->Path;
                for (i32 j = 0; j < 39 && *src; ++j, ++src, ++dst)
                    *dst = (*src >= 'A' && *src <= 'Z') ? *src + ('a' - 'A') : *src;
                *dst = '\0';

                arts_sprintf(info->Description, sizeof(info->Description), "%s", f->Path);
                arts_strcpy(info->Colors, sizeof(info->Colors), "RED,BLUE,YELLOW,GREY,WHITE,BLACK,GREEN,PURPLE");
                info->Flags = 0;
                info->Order = NumVehicles;
                info->Valid = true;
                info->Horsepower = 200;
                info->TopSpeed = 140;
                info->Durability = 5;
                info->Mass = 1500;
                info->ScoringBias = 1.0f;

                Ptr<mmVehInfo*[]> vehicles = arnewa mmVehInfo*[NumVehicles + 1];
                for (i32 j = 0; j < NumVehicles; ++j)
                    vehicles[j] = Vehicles[j];
                vehicles[NumVehicles] = info.release();
                Vehicles.swap(vehicles);
                ++NumVehicles;
            }
        }
    }

    SetDefaultVehicle("vpbug");
}

void mmVehList::Print()
{
    for (i32 i = 0; i < NumVehicles; ++i)
    {
        Displayf("******VEHICLE # %d", i + 1);
        Vehicles[i]->Print();
        Displayf("");
    }
}

void mmVehList::SetDefaultVehicle(const char* name)
{
    DefaultVehicle = GetVehicleInfo(name);

    ArAssert(DefaultVehicle, "Invalid Default Vehicle");
}