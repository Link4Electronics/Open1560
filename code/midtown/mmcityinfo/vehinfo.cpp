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

define_dummy_symbol(mmcityinfo_vehinfo);

#include "vehinfo.h"

#include "stream/fsystem.h"
#include "stream/stream.h"

#include <cstdio>
#include <cstring>

mmVehInfo::mmVehInfo()
{
    std::memset(reinterpret_cast<char*>(this) + sizeof(void*), 0, sizeof(*this) - sizeof(void*));
}

i32 mmVehInfo::Load(char* filename)
{
    Owner<Stream> stream = FileSystem::OpenAny(filename, true, nullptr, 0);

    if (!stream)
        return 0;

    char buffer[256];

    while (stream->Gets(buffer, sizeof(buffer)))
    {
        char* newline = std::strchr(buffer, '\r');
        if (newline)
            *newline = '\0';
        newline = std::strchr(buffer, '\n');
        if (newline)
            *newline = '\0';

        char key[64];
        char value[192];

        if (std::sscanf(buffer, " %63[^=]=%191[^\r\n]", key, value) < 2)
            continue;

        if (!std::strcmp(key, "BaseName"))
        {
            arts_strcpy(BaseName, sizeof(BaseName), value);
        }
        else if (!std::strcmp(key, "Description"))
        {
            arts_strcpy(Description, sizeof(Description), value);
        }
        else if (!std::strcmp(key, "Colors"))
        {
            arts_strcpy(Colors, sizeof(Colors), value);
        }
        else if (!std::strcmp(key, "Flags"))
        {
            std::sscanf(value, "%d", &Flags);
        }
        else if (!std::strcmp(key, "Order"))
        {
            std::sscanf(value, "%d", &Order);
        }
        else if (!std::strcmp(key, "ScoringBias"))
        {
            std::sscanf(value, "%f", &ScoringBias);
        }
        else if (!std::strcmp(key, "UnlockScore"))
        {
            std::sscanf(value, "%d", &UnlockScore);
        }
        else if (!std::strcmp(key, "UnlockFlags"))
        {
            std::sscanf(value, "%d", &UnlockFlags);
        }
        else if (!std::strcmp(key, "Horsepower"))
        {
            std::sscanf(value, "%d", &Horsepower);
        }
        else if (!std::strcmp(key, "Top Speed"))
        {
            std::sscanf(value, "%d", &TopSpeed);
        }
        else if (!std::strcmp(key, "Durability"))
        {
            std::sscanf(value, "%d", &Durability);
        }
        else if (!std::strcmp(key, "Mass"))
        {
            std::sscanf(value, "%d", &Mass);
        }
    }

    if (BaseName[0] == '\0')
        return 0;

    Valid = true;

    return 1;
}

i32 mmVehInfo::IsValid()
{
    return Valid ? 1 : 0;
}

void mmVehInfo::Print()
{
    Displayf("mmVehInfo: %s", BaseName);
    Displayf("  Description: %s", Description);
    Displayf("  Colors: %s", Colors);
    Displayf("  Flags: %d  Order: %d", Flags, Order);
    Displayf("  Horsepower: %d  Top Speed: %d  Durability: %d  Mass: %d", Horsepower, TopSpeed, Durability, Mass);
    Displayf("  ScoringBias: %f  Valid: %d", ScoringBias, Valid);
}
