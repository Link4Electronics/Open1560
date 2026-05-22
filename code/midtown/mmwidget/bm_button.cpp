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

define_dummy_symbol(mmwidget_bm_button);

#include "bm_button.h"

#include "agi/pipeline.h"
#include "arts7/cullmgr.h"
#include "eventq7/eventq.h"

UIBMButton::UIBMButton()
{
    Enabled = 1;
    Active = false;
    ReadOnly = 0;
    MouseHit = 0;
}

UIBMButton::~UIBMButton()
{
    Kill();
}

void UIBMButton::Init(char* name, f32 x, f32 y, i32 /*type*/, i32 /*arg5*/, i32* /*arg6*/, i32 /*arg7*/, i32 /*arg8*/,
    LocString* /*arg9*/, Callback /*cb_1*/, Callback /*cb_2*/)
{
    Label = name;
    X = x;
    Y = y;
    MinX = x;
    MinY = y;
    MaxX = x + 0.12f;
    MaxY = y + 0.05f;
    Width = MaxX - MinX;
    Height = MaxY - MinY;
}

void UIBMButton::Update()
{
    CullMgr()->DeclareCullable2D(this);
}

void UIBMButton::Cull()
{
    if (!Enabled)
        return;

    i32 x = static_cast<i32>(MinX * Pipe()->GetWidth());
    i32 y = static_cast<i32>(MinY * Pipe()->GetHeight());
    i32 w = static_cast<i32>((MaxX - MinX) * Pipe()->GetWidth());
    i32 h = static_cast<i32>((MaxY - MinY) * Pipe()->GetHeight());

    u32 color = Active ? 0xFF4488FF : 0xFFFFAA00;
    Pipe()->ClearRect(x, y, w, h, color);
}

void UIBMButton::Kill()
{
}

void UIBMButton::Action(eqEvent /*arg1*/)
{
}

void UIBMButton::Switch(b32 arg1)
{
    Active = arg1;
}

void UIBMButton::Enable()
{
    Enabled = 1;
}

void UIBMButton::Disable()
{
    Enabled = 0;
}

void UIBMButton::SetPosition(f32 arg1, f32 arg2)
{
    f32 dx = arg1 - X;
    f32 dy = arg2 - Y;
    X = arg1;
    Y = arg2;
    MinX += dx;
    MinY += dy;
    MaxX += dx;
    MaxY += dy;
}

char* UIBMButton::ReturnDescription()
{
    return const_cast<char*>(Label);
}

f32 UIBMButton::GetScreenHeight()
{
    return MaxY - MinY;
}

MetaClass* UIBMButton::GetClass()
{
    return nullptr;
}

void UIBMButton::LoadBitmap(char* /*arg1*/)
{
}

void UIBMButton::GetSize()
{
}

void UIBMButton::PlaySound()
{
}

void UIBMButton::AllocateSounds()
{
}

AudSound* UIBMButton::s_pSound {nullptr};

void UIBMButton::DeclareFields()
{
}

void UIBMButton::GetHitArea(f32& arg1, f32& arg2)
{
    arg1 = 0.0f;
    arg2 = 0.0f;
}

i32 UIBMButton::GetDiv()
{
    return 0;
}

void UIBMButton::DoToggle()
{
}

void UIBMButton::MexOff()
{
}

void UIBMButton::MexOn()
{
}

void UIBMButton::Unkill()
{
}

agiBitmap* UIBMButton::CreateDummyBitmap()
{
    return nullptr;
}
