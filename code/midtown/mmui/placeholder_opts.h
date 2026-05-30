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

#include "mmwidget/menu.h"

#include "agi/bitmap.h"

enum
{
    IDC_PLACEHOLDER_DONE = 200,
};

class AudioOptionMenu final : public UIMenu
{
public:
    ARTS_EXPORT AudioOptionMenu(i32 menu_id);
    ~AudioOptionMenu() override = default;
    void PreSetup() override;
    void PostSetup() override;
};

class GraphicsOptionMenu final : public UIMenu
{
public:
    ARTS_EXPORT GraphicsOptionMenu(i32 menu_id);
    ~GraphicsOptionMenu() override = default;
    void PreSetup() override;
    void PostSetup() override;
};

class ControlOptionMenu final : public UIMenu
{
public:
    ARTS_EXPORT ControlOptionMenu(i32 menu_id);
    ~ControlOptionMenu() override = default;
    void PreSetup() override;
    void PostSetup() override;
};

class AboutOptionMenu final : public UIMenu
{
public:
    ARTS_EXPORT AboutOptionMenu(i32 menu_id);
    ~AboutOptionMenu() override = default;
    void PreSetup() override;
    void PostSetup() override;
    void Update() override;
    void Cull() override;

private:
    Rc<agiBitmap> credits_;
    f32 credits_start_ {0.0f};
    i32 credits_x_ {0};
    i32 credits_y_ {0};
    i32 credits_width_ {0};
    i32 credits_height_ {0};
    i32 credits_vheight_ {0};
    i32 credits_scroll_ {0};
};
