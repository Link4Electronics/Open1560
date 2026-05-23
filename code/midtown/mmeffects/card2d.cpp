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

define_dummy_symbol(mmeffects_card2d);

#include "card2d.h"

#include <algorithm>

#include "agi/pipeline.h"
#include "arts7/cullmgr.h"
#include "vector7/vector4.h"

Card2D::Card2D() = default;

Card2D::~Card2D() = default;

void Card2D::Init(asCamera* camera, f32 x, f32 y, f32 w, f32 h, f32 alpha)
{
    camera_ = camera;
    x_ = x;
    y_ = y;
    w_ = w;
    h_ = h;
    alpha_ = alpha;
}

void Card2D::SetColor(Vector4) {} // ARTS_IMPORT stub

void Card2D::SetDimensions(f32 x, f32 y, f32 w, f32 h)
{
    x_ = x;
    y_ = y;
    w_ = w;
    h_ = h;
}

void Card2D::SetPosition(f32 x, f32 y)
{
    x_ = x;
    y_ = y;
}

void Card2D::SetAlpha(f32 alpha)
{
    alpha_ = alpha;
}

void Card2D::Update()
{
    asNode::Update();
    CullMgr()->DeclareCullable2D(this);
}

void Card2D::Cull()
{
    i32 x = static_cast<i32>(x_ * Pipe()->GetWidth());
    i32 y = static_cast<i32>(y_ * Pipe()->GetHeight());
    i32 w = static_cast<i32>(w_ * Pipe()->GetWidth());
    i32 h = static_cast<i32>(h_ * Pipe()->GetHeight());
    u8 a = static_cast<u8>(std::clamp(alpha_, 0.0f, 1.0f) * 255.0f);
    Pipe()->ClearRect(x, y, w, h, (a << 24) | 0x000000);
}
