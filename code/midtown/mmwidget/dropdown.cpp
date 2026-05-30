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

define_dummy_symbol(mmwidget_dropdown);

#include "dropdown.h"

#include "data7/metadefine.h"
#include "mmeffects/mmtext.h"

#define MM_DROP_TEXT_EFFECTS (MM_TEXT_VCENTER | MM_TEXT_PADDING | MM_TEXT_REQUIRED)

mmDropDown::mmDropDown() = default;

mmDropDown::~mmDropDown() = default;

void mmDropDown::Init(asCamera* camera, void* font, f32 x, f32 y, f32 w, f32 h, string options, i32)
{
    Camera = camera;
    Font = font;
    X = x;
    Y = y;
    Height = h;
    Width = w;
    Bottom = y + h; // items start below button

    InitString(std::move(options));
}

i32 mmDropDown::FindFirstEnabled()
{
    for (i32 i = 0; i < NumValues; ++i)
    {
        if (!(DisabledMask & (1 << i)))
            return i;
    }
    return -1;
}

void mmDropDown::GetCurrentString(char* buf, i32 size)
{
    if (Highlighted >= 0 && Highlighted < NumValues)
    {
        string value = ValuesString.SubString(Highlighted + 1);
        arts_strncpy(buf, value.get(), size);
    }
    else
    {
        if (size > 0)
            buf[0] = '\0';
    }
}

i32 mmDropDown::GetHit(f32 arg1, f32 arg2)
{
    if (NumValues <= 0 || Height <= 0.0f)
        return -1;

    f32 hit_y = arg2 - Bottom;
    i32 index = static_cast<i32>(hit_y / Height);

    if (index < 0 || index >= NumValues)
        return -1;

    return index;
}

void mmDropDown::SetDisabledColors()
{
    for (i32 i = 0; i < NumValues; ++i)
    {
        if (DisabledMask & (1 << i))
        {
            ValueNodes[i].SetEffects(0, MM_DROP_TEXT_EFFECTS);
        }
    }
}

void mmDropDown::SetString(string arg1)
{
    ValuesString = std::move(arg1);
}

void mmDropDown::Update()
{
    if (!Enabled)
        return;

    asNode::Update();
}

void mmDropDown::InitString(string values)
{
    ValueNodes = nullptr;
    DropIndex = nullptr;

    NumValues = values.NumSubStrings();
    ValueNodes = arnewa mmTextNode[NumValues] {};
    DropIndex = arnewa u32[NumValues] {};

    DropHeight = NumValues * Height;
    Highlighted = -1;

    SetString(std::move(values));

    for (i32 i = 0; i < NumValues; ++i)
    {
        mmTextNode* node = &ValueNodes[i];
        string value = ValuesString.SubString(i + 1);

        AddChild(node);
        node->Init(X, Bottom + (i * Height), Width, Height, 1, 0);
        DropIndex[i] = node->AddText(Font, value.get_loc(), MM_DROP_TEXT_EFFECTS, 0.0f, 0.0f);
    }

    if (DisabledMask)
        SetDisabledColors();
}

void mmDropDown::SetHighlight(i32 index)
{
    if (Highlighted >= 0 && Highlighted < NumValues)
    {
        ValueNodes[Highlighted].SetEffects(0, MM_DROP_TEXT_EFFECTS);
    }

    if (index >= 0 && index < NumValues)
    {
        i32 effects = MM_DROP_TEXT_EFFECTS | MM_TEXT_BORDER;

        if (!(DisabledMask & (1 << index)))
            effects |= MM_TEXT_HIGHLIGHT;

        ValueNodes[index].SetEffects(0, effects);
    }

    Highlighted = index;
}

META_DEFINE_CHILD("mmDropDown", mmDropDown, asNode)
{}