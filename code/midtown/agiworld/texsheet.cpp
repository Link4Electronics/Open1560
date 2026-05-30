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

define_dummy_symbol(agiworld_texsheet);

#include "texsheet.h"

#include "stream/fsystem.h"
#include "stream/stream.h"

#include <cstdlib>
#include <cstring>

// Strong TEXSHEET object overrides the weak stub (correct size for 64-bit)
agiTexSheet TEXSHEET;

static i32 cmpTex(const void* arg1, const void* arg2)
{
    return arts_stricmp(*static_cast<char* const*>(arg1), *static_cast<char* const*>(arg2));
}

static u32 ParseFlags(const char* str)
{
    u32 flags = 0;

    for (const char* p = str; *p; ++p)
    {
        switch (*p)
        {
            case 'w': flags |= agiTexProp::Snowable; break;
            case 'g': flags |= agiTexProp::AlphaGlow; break;
            case 'l': flags |= agiTexProp::Lightmap; break;
            case 's': flags |= agiTexProp::Shadow; break;
            case 't': flags |= agiTexProp::Transparent; break;
            case 'k': flags |= agiTexProp::Chromakey; break;
            case 'n': flags |= agiTexProp::NotLit; break;
            case 'd': flags |= agiTexProp::DullOrDamaged; break;
            case 'u': flags |= agiTexProp::ClampUOrBoth; break;
            case 'v': flags |= agiTexProp::ClampVOrBoth; break;
            case 'c': flags |= agiTexProp::ClampBoth; break;
            case 'U': flags |= agiTexProp::ClampUOrNeither; break;
            case 'V': flags |= agiTexProp::ClampVOrNeither; break;
            case 'e': flags |= agiTexProp::RoadFloorCeiling; break;
            case 'm': flags |= agiTexProp::AlwaysModulate; break;
            case 'p': flags |= agiTexProp::AlwaysPerspCorrect; break;
        }
    }

    return flags;
}

static b32 ParseHex32(const char* str, u32& value)
{
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
        str += 2;

    if (*str == '\0')
        return false;

    u32 result = 0;

    for (; *str; ++str)
    {
        char c = *str;
        result <<= 4;

        if (c >= '0' && c <= '9')
            result += c - '0';
        else if (c >= 'a' && c <= 'f')
            result += c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            result += c - 'A' + 10;
        else
            return false;
    }

    value = result;
    return true;
}

void agiTexSheet::Load(const char* filename)
{
    Owner<Stream> stream = FileSystem::OpenAny(filename, true, nullptr, 0);
    if (!stream)
        return;

    char buffer[256];

    // First pass: count lines
    i32 line_count = 0;
    i32 old_count = prop_count_;

    // First pass: count lines
    while (stream->Gets(buffer, sizeof(buffer)))
    {
        if (buffer[0] != '\0' && buffer[0] != '\n' && buffer[0] != '\r')
            ++line_count;
    }

    if (line_count <= 0)
        return;

    // Seek back to start
    stream->Seek(0);

    // Allocate new property array (existing props + new props)
    i32 new_count = old_count + line_count;
    agiTexProp* new_props = new agiTexProp[new_count];

    // Copy existing props
    if (props_)
    {
        for (i32 i = 0; i < old_count; ++i)
            new_props[i] = props_[i];
        delete[] props_;
    }

    props_ = new_props;

    // Second pass: parse each line
    while (stream->Gets(buffer, sizeof(buffer)))
    {
        // Strip trailing whitespace/newlines
        char* end = buffer + std::strlen(buffer);
        while (end > buffer && (end[-1] == '\n' || end[-1] == '\r' || end[-1] == ' ' || end[-1] == '\t'))
            --end;
        *end = '\0';

        if (buffer[0] == '\0')
            continue;

        agiTexProp& prop = props_[prop_count_];

        // Tokenize by whitespace
        char* tokens[16];
        i32 token_count = 0;

        char* pos = buffer;
        while (*pos && token_count < 16)
        {
            while (*pos == ' ' || *pos == '\t')
                ++pos;
            if (*pos == '\0')
                break;
            tokens[token_count++] = pos;
            while (*pos && *pos != ' ' && *pos != '\t')
                ++pos;
            if (*pos)
                *pos++ = '\0';
        }

        if (token_count < 1)
            continue;

        prop.Name = arts_strdup(tokens[0]);

        if (prop.Name)
        {
            char* upper = prop.Name;
            while (*upper) { *upper = std::toupper(*upper); ++upper; }
        }

        // Tokens 1-3: High, Medium, Low pack shift levels
        if (token_count > 1) prop.High = static_cast<u8>(std::atoi(tokens[1]));
        if (token_count > 2) prop.Medium = static_cast<u8>(std::atoi(tokens[2]));
        if (token_count > 3) prop.Low = static_cast<u8>(std::atoi(tokens[3]));

        // Token 4: flags string
        if (token_count > 4) prop.Flags = ParseFlags(tokens[4]);

        // Token 5: alternate name (optional)
        if (token_count > 5)
        {
            prop.AlternateName = arts_strdup(tokens[5]);
            if (prop.AlternateName)
            {
                char* upper = prop.AlternateName;
                while (*upper) { *upper = std::toupper(*upper); ++upper; }
            }
        }

        // Token 6: sibling name (optional)
        if (token_count > 6)
        {
            prop.Sibling = arts_strdup(tokens[6]);
            if (prop.Sibling)
            {
                char* upper = prop.Sibling;
                while (*upper) { *upper = std::toupper(*upper); ++upper; }
            }
        }

        // Token 7: width (optional)
        if (token_count > 7) prop.Width = static_cast<u16>(std::atoi(tokens[7]));

        // Token 8: height (optional)
        if (token_count > 8) prop.Height = static_cast<u16>(std::atoi(tokens[8]));

        // Token 9: color in hex (optional)
        if (token_count > 9) ParseHex32(tokens[9], prop.Color);

        ++prop_count_;
    }

    // Sort by name
    if (prop_count_ > old_count)
        qsort(props_ + old_count, prop_count_ - old_count, sizeof(agiTexProp), cmpTex);

    // Update count to reflect total entries
    prop_count_ = new_count;
}

void agiTexSheet::Kill()
{
    if (props_)
    {
        for (i32 i = 0; i < prop_count_; ++i)
        {
            arts_free(props_[i].Name);
            arts_free(props_[i].AlternateName);
            arts_free(props_[i].Sibling);
        }

        delete[] props_;
        props_ = nullptr;
    }

    prop_count_ = 0;
}

agiTexProp* agiTexSheet::Lookup(const char* name, i32 variation)
{
    if (!props_ || prop_count_ <= 0 || !name)
        return nullptr;

    // Binary search
    i32 start = 0;
    i32 end = prop_count_ - 1;

    while (start <= end)
    {
        i32 mid = (start + end) >> 1;
        i32 cmp = arts_stricmp(props_[mid].Name, name);

        if (cmp == 0)
            return &props_[mid];

        if (cmp < 0)
            start = mid + 1;
        else
            end = mid - 1;
    }

    return nullptr;
}

i32 agiTexSheet::GetVariationCount(const char* name)
{
    agiTexProp* prop = Lookup(name, 0);

    if (!prop || !prop->AlternateName)
        return 1;

    i32 count = 1;
    const char* alt = prop->AlternateName;

    while (*alt)
    {
        if (*alt == '|')
            ++count;
        ++alt;
    }

    return count;
}

char* agiTexSheet::RemapName(const char* name, i32 variation)
{
    if (variation <= 0 || !name)
        return const_cast<char*>(name);

    agiTexProp* prop = Lookup(name, 0);

    if (!prop || !prop->AlternateName)
        return const_cast<char*>(name);

    const char* alt = prop->AlternateName;
    i32 count = 0;

    while (count < variation - 1 && *alt)
    {
        if (*alt == '|')
            ++count;
        ++alt;
    }

    if (count < variation - 1)
        return const_cast<char*>(name);

    if (count == variation - 1 && *alt == '|')
        ++alt;

    static char temp[64];
    i32 len = 0;
    while (*alt && *alt != '|' && len < 63)
        temp[len++] = *alt++;
    temp[len] = '\0';

    return temp;
}
