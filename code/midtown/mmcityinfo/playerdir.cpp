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

define_dummy_symbol(mmcityinfo_playerdir);

#include "playerdir.h"

#include "stream/stream.h"

#include <cstdio>
#include <cstring>

static constexpr i32 MAX_PLAYERS = 18;
static constexpr i32 NAME_LEN = 40;

struct PlayerStorage
{
    i32 Count = 0;
    char Names[MAX_PLAYERS][NAME_LEN] {};
    char Files[MAX_PLAYERS][NAME_LEN] {};
    i32 LastPlayer = -1;
};

static PlayerStorage& GetStorage()
{
    static PlayerStorage s;
    return s;
}

static void MakePlayerPath(const char* base, char* out, usize out_size)
{
    if (base && *base)
    {
        arts_snprintf(out, out_size, "%s/players.dir", base);
    }
    else
    {
        arts_strcpy(out, "players.dir");
    }
}

mmPlayerDirectory::mmPlayerDirectory()
{
    std::memset(gap88, 0, sizeof(gap88));
}

mmPlayerDirectory::~mmPlayerDirectory() = default;

MetaClass* mmPlayerDirectory::GetClass()
{
    return mmInfoBase::GetClass();
}

i32 mmPlayerDirectory::AddPlayer(char* name)
{
    auto& s = GetStorage();

    if (s.Count >= MAX_PLAYERS)
        return -1;

    // Check for duplicate
    for (i32 i = 0; i < s.Count; ++i)
    {
        if (!std::strcmp(s.Names[i], name))
            return -1;
    }

    arts_strcpy(s.Names[s.Count], name);
    arts_snprintf(s.Files[s.Count], NAME_LEN, "player%d", s.Count);
    s.LastPlayer = s.Count;
    return s.Count++;
}

i32 mmPlayerDirectory::FindPlayer(char* name)
{
    auto& s = GetStorage();

    for (i32 i = 0; i < s.Count; ++i)
    {
        if (!std::strcmp(s.Names[i], name))
            return i;
    }

    return -1;
}

char* mmPlayerDirectory::GetFileName(char* name)
{
    i32 index = FindPlayer(name);
    if (index < 0)
        return nullptr;

    return GetFileName(index);
}

char* mmPlayerDirectory::GetFileName(i32 index)
{
    auto& s = GetStorage();

    if (index < 0 || index >= s.Count)
        return nullptr;

    return s.Files[index];
}

char* mmPlayerDirectory::GetLastPlayer()
{
    auto& s = GetStorage();

    if (s.LastPlayer < 0 || s.LastPlayer >= s.Count)
        return nullptr;

    return s.Names[s.LastPlayer];
}

i32 mmPlayerDirectory::GetNumPlayers()
{
    return GetStorage().Count;
}

char* mmPlayerDirectory::GetPlayer(i32 index)
{
    auto& s = GetStorage();

    if (index < 0 || index >= s.Count)
        return nullptr;

    return s.Names[index];
}

i32 mmPlayerDirectory::Load(char* path)
{
    char full_path[256];
    MakePlayerPath(path, full_path, sizeof(full_path));

    Ptr<Stream> file {arts_fopen(full_path, "r")};
    if (!file)
    {
        // No player directory yet — create default player
        Displayf("mmPlayerDirectory: No players.dir found, creating default");
        auto& s = GetStorage();
        s.Count = 0;
        s.LastPlayer = -1;
        AddPlayer(const_cast<char*>("DriverX"));
        return 1;
    }

    auto& s = GetStorage();
    s.Count = 0;
    s.LastPlayer = -1;

    char line[256];

    // First line: count
    if (file->Gets(line, sizeof(line)) <= 0)
    {
        s.Count = 0;
        // Default player
        AddPlayer(const_cast<char*>("DriverX"));
        return 1;
    }

    i32 count = 0;
    std::sscanf(line, "%d", &count);

    if (count > MAX_PLAYERS)
        count = MAX_PLAYERS;

    for (i32 i = 0; i < count; ++i)
    {
        if (file->Gets(line, sizeof(line)) <= 0)
            break;

        char name[NAME_LEN] {};
        char fname[NAME_LEN] {};
        std::sscanf(line, "%39s %39s", name, fname);

        if (name[0])
        {
            arts_strcpy(s.Names[s.Count], name);
            arts_strcpy(s.Files[s.Count], fname[0] ? fname : name);
            ++s.Count;
        }
    }

    if (s.Count == 0)
    {
        AddPlayer(const_cast<char*>("DriverX"));
    }

    return 1;
}

i32 mmPlayerDirectory::LoadBinary(char* arg1)
{
    return Load(arg1);
}

void mmPlayerDirectory::NewDirectory(i32 /*arg1*/)
{
    auto& s = GetStorage();
    s.Count = 0;
    s.LastPlayer = -1;
}

i32 mmPlayerDirectory::RemovePlayer(char* name)
{
    auto& s = GetStorage();

    i32 index = FindPlayer(name);
    if (index < 0)
        return 0;

    for (i32 i = index; i < s.Count - 1; ++i)
    {
        arts_strcpy(s.Names[i], s.Names[i + 1]);
        arts_strcpy(s.Files[i], s.Files[i + 1]);
    }

    --s.Count;

    if (s.LastPlayer >= s.Count)
        s.LastPlayer = s.Count - 1;

    return 1;
}

i32 mmPlayerDirectory::Save(char* path, i32 /*arg2*/)
{
    auto& s = GetStorage();

    char full_path[256];
    MakePlayerPath(path, full_path, sizeof(full_path));

    Ptr<Stream> file {arts_fopen(full_path, "w")};
    if (!file)
    {
        Warningf("mmPlayerDirectory: Could not write %s", full_path);
        return 0;
    }

    arts_fprintf(file.get(), "%d\n", s.Count);

    for (i32 i = 0; i < s.Count; ++i)
    {
        arts_fprintf(file.get(), "%s %s\n", s.Names[i], s.Files[i]);
    }

    return 1;
}

i32 mmPlayerDirectory::SaveBinary(char* arg1)
{
    return Save(arg1, 0);
}

void mmPlayerDirectory::SetLastPlayer(char* name)
{
    auto& s = GetStorage();

    i32 index = FindPlayer(name);
    if (index >= 0)
        s.LastPlayer = index;
}

void mmPlayerDirectory::SetPlayer(i32 index, char* name, char* fname)
{
    auto& s = GetStorage();

    if (index < 0 || index >= s.Count)
        return;

    arts_strcpy(s.Names[index], name);
    arts_strcpy(s.Files[index], fname);
}

char* mmPlayerDirectory::MakeFileName()
{
    // Generate the full path to the players.dir file
    static char result[256];
    MakePlayerPath(FilePath, result, sizeof(result));
    return result;
}
