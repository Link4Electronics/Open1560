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

#include "widget.h"

#include "core/refcount.h"

class agiBitmap;

class UIIcon final : public uiWidget
{
public:
    ARTS_IMPORT UIIcon();
    ARTS_EXPORT ~UIIcon() override = default;

    ARTS_IMPORT agiBitmap* CreateDummyBitmap();
    ARTS_IMPORT void Cull() override;
    ARTS_IMPORT void GetHitArea(f32& arg1, f32& arg2);
    ARTS_IMPORT void Init(char* arg1, f32 arg2, f32 arg3);
    ARTS_IMPORT void LoadBitmap(char* arg1);
    ARTS_IMPORT void Switch(b32 arg1) override;
    ARTS_IMPORT void Update() override;

    i32 dst_x_ {0};
    i32 dst_y_ {0};
    Rc<agiBitmap> bitmap_;
};
