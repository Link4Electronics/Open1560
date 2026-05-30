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

#include "localize/localize.h"
#include "widget.h"

class TextDropWidget;
class mmTextNode;
class UIIcon;
class agiBitmap;

class UITextDropdown final : public uiWidget
{
public:
    UITextDropdown();
    ~UITextDropdown() override;

    void Action(eqEvent arg1) override;
    void AssignString(string options);
    void CaptureAction(eqEvent arg1) override;
    void Cull() override;
    f32 GetScreenHeight() override;
    void Init(LocString* arg1, i32* arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, string arg7, i32 arg8, i32 arg9,
        i32 arg10, Callback arg11, char* arg12);
    void SetData(i32* arg1);
    void SetDisabledMask(ilong arg1);
    void SetPos(f32 arg1, f32 arg2);
    void SetSliderFocus(i32 arg1);
    void SetText(LocString* arg1);
    void UpdateValueText();
    i32 SetValue(i32 arg1);
    void Switch(b32 arg1) override;
    void Update() override;

    TextDropWidget* DropWidget {nullptr};
    i32* ValuePtr {nullptr};
    i32 StoredValue {0};
    mmTextNode* TextNode {nullptr};
    mmTextNode* ValueNode {nullptr};
    UIIcon* ArrowIcon {nullptr};
    i32 FontSize {16};
};
