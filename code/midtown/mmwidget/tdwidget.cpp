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

define_dummy_symbol(mmwidget_tdwidget);

#include "tdwidget.h"
#include "data7/str.h"

TextDropWidget::TextDropWidget() = default;
TextDropWidget::~TextDropWidget() = default;

void TextDropWidget::Init(
    asCamera* /*arg1*/, void* /*arg2*/, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7, string arg8, i32 arg9)
{
    PosX = arg3;
    PosY = arg4;
    SizeW = arg5;
    SizeH = arg6;
    Options = std::move(arg8);
    FontSize = arg9;
}

i32 TextDropWidget::IncDrop()
{
    if (DisabledMask)
        return -1;

    i32 count = Options.NumSubStrings();

    if (count <= 0)
        return -1;

    CurrentValue = (CurrentValue + 1) % count;
    return CurrentValue;
}

i32 TextDropWidget::DecDrop()
{
    if (DisabledMask)
        return -1;

    i32 count = Options.NumSubStrings();

    if (count <= 0)
        return -1;

    CurrentValue = (CurrentValue - 1 + count) % count;
    return CurrentValue;
}

i32 TextDropWidget::Capture(f32 /*arg1*/, f32 /*arg2*/)
{
    return 0;
}

MetaClass* TextDropWidget::GetClass()
{
    return nullptr;
}

ilong TextDropWidget::GetDisabledMask()
{
    return DisabledMask;
}

i32 TextDropWidget::IsActive()
{
    return Active;
}

void TextDropWidget::SetActive(i32 arg1)
{
    Active = arg1;
    ActivateNode();
}

void TextDropWidget::SetDisabledMask(ilong arg1)
{
    DisabledMask = arg1;
}

i32 TextDropWidget::SetHighlight(i32 arg1)
{
    i32 old = Highlight;
    Highlight = arg1;
    return old;
}

void TextDropWidget::SetString(string arg1)
{
    Options = std::move(arg1);
    CurrentValue = 0;
}

i32 TextDropWidget::SetValue(i32 arg1)
{
    i32 old = CurrentValue;
    CurrentValue = arg1;
    return old;
}

void TextDropWidget::Switch(i32 /*arg1*/, Vector4& /*arg2*/)
{
    ActivateNode();
}

void TextDropWidget::Update()
{
    asNode::Update();
}

void TextDropWidget::DeclareFields()
{}
