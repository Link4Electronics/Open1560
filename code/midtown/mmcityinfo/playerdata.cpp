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

define_dummy_symbol(mmcityinfo_playerdata);

#include "playerdata.h"

#include "data7/metadefine.h"
#include "state.h"

#include "stream/stream.h"

// Strong definition of the current player global (overrides weak stub in game_stubs.cpp)
mmPlayerData MMCURRPLAYER {};

#include <cstdio>
#include <cstring>

mmPlayerData::mmPlayerData()
{
    std::memset(PlayerName, 0, sizeof(PlayerName));
    std::memset(NetName, 0, sizeof(NetName));
    std::memset(FileName, 0, sizeof(FileName));
    Difficulty = 0;
    Progress = 0;
    std::memset(LastCarPicked, 0, sizeof(LastCarPicked));
    LastCarColor = 0;
    LastGamePicked = 0;
    LastRacePicked = 0;
    Loaded = false;
}

mmPlayerData::~mmPlayerData() = default;

static void MakePlayerPath(const char* base, const char* fname, const char* ext, char* out, usize out_size)
{
    if (base && *base)
    {
        arts_snprintf(out, out_size, "%s/%s.%s", base, fname, ext);
    }
    else
    {
        arts_snprintf(out, out_size, "%s.%s", fname, ext);
    }
}

i32 mmPlayerData::Load(char* path)
{
    char full_path[256];
    MakePlayerPath(path, FileName[0] ? FileName : PlayerName, "sav", full_path, sizeof(full_path));

    Ptr<Stream> file {arts_fopen(full_path, "r")};
    if (!file)
    {
        Warningf("mmPlayerData: Could not load %s", full_path);
        return 0;
    }

    char line[256];

    while (file->Gets(line, sizeof(line)) > 0)
    {
        // Remove trailing newline
        usize len = std::strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';

        char key[64] {};
        char value[128] {};
        i32 int_val = 0;

        if (std::sscanf(line, "%63s %d", key, &int_val) >= 2)
        {
            if (!std::strcmp(key, "Difficulty"))
                Difficulty = static_cast<i8>(int_val);
            else if (!std::strcmp(key, "Progress"))
                Progress = int_val;
            else if (!std::strcmp(key, "LastCarColor"))
                LastCarColor = int_val;
            else if (!std::strcmp(key, "LastGamePicked"))
                LastGamePicked = int_val;
            else if (!std::strcmp(key, "LastRacePicked"))
                LastRacePicked = int_val;
        }

        if (std::sscanf(line, "%63s %127[^\n]", key, value) >= 2)
        {
            // Strip quotes if present
            char* v = value;
            if (*v == '"')
            {
                ++v;
                usize vlen = std::strlen(v);
                if (vlen > 0 && v[vlen - 1] == '"')
                    v[vlen - 1] = '\0';
            }

            if (!std::strcmp(key, "PlayerName"))
                arts_strcpy(PlayerName, v);
            else if (!std::strcmp(key, "NetName"))
                arts_strcpy(NetName, v);
            else if (!std::strcmp(key, "FileName"))
                arts_strcpy(FileName, v);
            else if (!std::strcmp(key, "LastCarPicked"))
                arts_strcpy(LastCarPicked, v);
        }
    }

    Loaded = true;
    return 1;
}

i32 mmPlayerData::LoadBinary(char* arg1)
{
    return Load(arg1);
}

i32 mmPlayerData::Save(char* path, i32 /*arg2*/)
{
    char full_path[256];
    MakePlayerPath(path, FileName[0] ? FileName : PlayerName, "sav", full_path, sizeof(full_path));

    Ptr<Stream> file {arts_fopen(full_path, "w")};
    if (!file)
    {
        Warningf("mmPlayerData: Could not write %s", full_path);
        return 0;
    }

    arts_fprintf(file.get(), "PlayerName \"%s\"\n", PlayerName);
    arts_fprintf(file.get(), "NetName \"%s\"\n", NetName);
    arts_fprintf(file.get(), "FileName \"%s\"\n", FileName);
    arts_fprintf(file.get(), "Difficulty %d\n", Difficulty);
    arts_fprintf(file.get(), "Progress %d\n", Progress);
    arts_fprintf(file.get(), "LastCarPicked \"%s\"\n", LastCarPicked);
    arts_fprintf(file.get(), "LastCarColor %d\n", LastCarColor);
    arts_fprintf(file.get(), "LastGamePicked %d\n", LastGamePicked);
    arts_fprintf(file.get(), "LastRacePicked %d\n", LastRacePicked);

    return 1;
}

i32 mmPlayerData::SaveBinary(char* arg1)
{
    return Save(arg1, 0);
}

char* mmPlayerData::GetFileName()
{
    return FileName;
}

char* mmPlayerData::GetName()
{
    return PlayerName;
}

char* mmPlayerData::GetNetName()
{
    return NetName;
}

i32 mmPlayerData::GetTotalPassed()
{
    return GetCheckpointPassed() + GetCircuitPassed() + GetBlitzPassed();
}

i32 mmPlayerData::GetTotalScore()
{
    return Progress;
}

i32 mmPlayerData::GetBlitzPassed()
{
    i32 count = 0;
    for (i32 i = 0; i < 12; ++i)
    {
        if (BlitzRecords[i].ComputeCRC() != 0)
            ++count;
    }
    return count;
}

i32 mmPlayerData::GetCheckpointPassed()
{
    i32 count = 0;
    for (i32 i = 0; i < 12; ++i)
    {
        if (CheckpointRecords[i].ComputeCRC() != 0)
            ++count;
    }
    return count;
}

i32 mmPlayerData::GetCircuitPassed()
{
    i32 count = 0;
    for (i32 i = 0; i < 12; ++i)
    {
        if (CircuitRecords[i].ComputeCRC() != 0)
            ++count;
    }
    return count;
}

void mmPlayerData::SetFileName(char* name)
{
    arts_strcpy(FileName, name);
}

void mmPlayerData::SetName(char* name)
{
    arts_strcpy(PlayerName, name);
}

void mmPlayerData::SetNetName(char* name)
{
    arts_strcpy(NetName, name);
}

void mmPlayerData::Reset()
{
    std::memset(PlayerName, 0, sizeof(PlayerName));
    std::memset(NetName, 0, sizeof(NetName));
    std::memset(FileName, 0, sizeof(FileName));
    Difficulty = 0;
    Progress = 0;
    std::memset(LastCarPicked, 0, sizeof(LastCarPicked));
    LastCarColor = 0;
    LastGamePicked = 0;
    LastRacePicked = 0;
    Loaded = false;
}

u32 mmPlayerData::ComputeCRC()
{
    return 0;
}

i32 mmPlayerData::GetBlitzMask()
{
    return 0;
}

i32 mmPlayerData::GetCheckpointMask()
{
    return 0;
}

i32 mmPlayerData::GetCheckpointProgress(i32 /*arg1*/)
{
    return 0;
}

i32 mmPlayerData::GetCircuitMask()
{
    return 0;
}

i32 mmPlayerData::RegisterFinish(mmPlayerRecord arg1, i32 arg2, i32 arg3)
{
    return 0;
}

i32 mmPlayerData::RegisterFinish(i32 /*arg1*/, f32 /*arg2*/, char /*arg3*/)
{
    return 0;
}

void mmPlayerData::ResolveCheckpointProgress()
{}

MetaClass* mmPlayerData::GetClass()
{
    return mmInfoBase::GetClass();
}

// mmPlayerRecord implementations

mmPlayerRecord::mmPlayerRecord()
{
    std::memset(gap88, 0, sizeof(gap88));
}

mmPlayerRecord::mmPlayerRecord(const mmPlayerRecord& arg1)
{
    std::memcpy(gap88, arg1.gap88, sizeof(gap88));
}

void mmPlayerRecord::operator=(mmPlayerRecord& arg1)
{
    std::memcpy(gap88, arg1.gap88, sizeof(gap88));
}

u32 mmPlayerRecord::ComputeCRC()
{
    // Simple CRC based on whether any non-zero data exists
    u32 crc = 0;
    for (usize i = 0; i < sizeof(gap88); ++i)
    {
        crc = (crc << 1) | (crc >> 31);
        crc ^= gap88[i];
    }
    return crc;
}

i32 mmPlayerRecord::LoadBinary(Stream* /*arg1*/)
{
    return 0;
}

i32 mmPlayerRecord::SaveBinary(Stream* /*arg1*/)
{
    return 0;
}

void mmPlayerRecord::Reset()
{
    std::memset(gap88, 0, sizeof(gap88));
}

MetaClass* mmPlayerRecord::GetClass()
{
    return mmInfoBase::GetClass();
}
