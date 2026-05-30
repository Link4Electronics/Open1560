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

define_dummy_symbol(mmwidget_icon);

#include "icon.h"

#include "agi/bitmap.h"
#include "agi/pipeline.h"
#include "arts7/cullmgr.h"

#include <cstdint>
#include <cstdio>

UIIcon::UIIcon() = default;

void UIIcon::Init(char* arg1, f32 arg2, f32 arg3)
{
    dst_x_ = static_cast<i32>(Pipe()->GetWidth() * arg2);
    dst_y_ = static_cast<i32>(Pipe()->GetHeight() * arg3);
    LoadBitmap(arg1);
}

void UIIcon::LoadBitmap(char* arg1)
{
    RcOwner<agiBitmap> bmp = Pipe()->GetBitmap(arg1, 0.0f, 0.0f, 1);

    if (bmp)
    {
        bitmap_ = as_rc std::move(bmp);
    }
    else
    {
        agiBitmap* dummy = CreateDummyBitmap();
        if (dummy)
            bitmap_ = Rc<agiBitmap>(dummy);
    }
}

void UIIcon::Cull()
{
    if (!bitmap_)
        return;

    Pipe()->CopyBitmap(dst_x_, dst_y_, bitmap_.get(), 0, 0, bitmap_->GetWidth(), bitmap_->GetHeight());
}

void UIIcon::GetHitArea(f32& arg1, f32& arg2)
{
    if (bitmap_)
    {
        arg1 = static_cast<f32>(bitmap_->GetWidth()) / Pipe()->GetWidth();
        arg2 = static_cast<f32>(bitmap_->GetHeight()) / Pipe()->GetHeight();
    }
    else
    {
        arg1 = 0.0f;
        arg2 = 0.0f;
    }
}

agiBitmap* UIIcon::CreateDummyBitmap()
{
    RcOwner<agiBitmap> result = Pipe()->CreateBitmap();
    if (!result)
        return nullptr;

    char name[32];
    std::sprintf(name, "*DummyBM:%08x", reinterpret_cast<std::uintptr_t>(this));
    result->Init(name, 50.0f, 50.0f, 0);
    return result.release();
}

void UIIcon::Switch(b32 arg1)
{
    if (arg1)
        ActivateNode();
    else
        DeactivateNode();
}

void UIIcon::Update()
{
    if (bitmap_)
        CullMgr()->DeclareBitmap(this, bitmap_.get());

    asNode::Update();
}
