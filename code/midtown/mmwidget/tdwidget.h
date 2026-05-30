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

#include "arts7/node.h"
#include "data7/str.h"

class asCamera;

class TextDropWidget final : public asNode
{
public:
    TextDropWidget();
    ~TextDropWidget() override;

    i32 Capture(f32 arg1, f32 arg2);
    i32 DecDrop();
    MetaClass* GetClass() override;
    ilong GetDisabledMask();
    i32 IncDrop();
    void Init(
        asCamera* arg1, void* arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7, string arg8, i32 arg9);
    i32 IsActive();
    void SetActive(i32 arg1);
    void SetDisabledMask(ilong arg1);
    i32 SetHighlight(i32 arg1);
    void SetString(string arg1);
    i32 SetValue(i32 arg1);
    void Switch(i32 arg1, Vector4& arg2);
    void Update() override;
    static void DeclareFields();

    string Options;
    i32 CurrentValue {0};
    i32 Active {0};
    i32 Highlight {-1};
    ilong DisabledMask {0};
    f32 PosX {0};
    f32 PosY {0};
    f32 SizeW {0};
    f32 SizeH {0};
    i32 FontSize {0};
};
