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

define_dummy_symbol(agi_cmodelx);

#include "cmodelx.h"

#include "rgba.h"
#include "surface.h"

static u32 ScaleComponent(u32 value, u32 bits)
{
    if (bits == 0)
        return 0;
    if (bits >= 8)
        return value;
    u32 max_in = 255;
    u32 max_out = (1 << bits) - 1;
    return (value * max_out + max_in / 2) / max_in;
}

static u32 PackColor(u32 r, u32 g, u32 b, u32 a, u32 shift_r, u32 shift_g, u32 shift_b, u32 shift_a, u32 bits_r, u32 bits_g, u32 bits_b, u32 bits_a)
{
    return (ScaleComponent(r, bits_r) << shift_r) |
           (ScaleComponent(g, bits_g) << shift_g) |
           (ScaleComponent(b, bits_b) << shift_b) |
           (ScaleComponent(a, bits_a) << shift_a);
}

static void WritePixel16(void* surface, i32 pitch, i32 x, i32 y, u32 color)
{
    reinterpret_cast<u16*>(static_cast<u8*>(surface) + (y * pitch))[x] = static_cast<u16>(color);
}

static void WritePixel32(void* surface, i32 pitch, i32 x, i32 y, u32 color)
{
    reinterpret_cast<u32*>(static_cast<u8*>(surface) + (y * pitch))[x] = color;
}

// Override weak stubs from game_stubs.cpp with properly constructed instances
agiColorModelRGB555 ColorModelRGB555;
agiColorModelRGB565 ColorModelRGB565;
agiColorModelRGB555_Rev ColorModelRGB555_Rev;
agiColorModelRGB565_Rev ColorModelRGB565_Rev;
agiColorModelRGB888 ColorModelRGB888;
agiColorModelRGB888_Rev ColorModelRGB888_Rev;
agiColorModelRGBA5551 ColorModelRGBA5551;
agiColorModelRGBA4444 ColorModelRGBA4444;
agiColorModelARGB ColorModelARGB;
agiColorModelABGR ColorModelABGR;

agiColorModelRGB555::agiColorModelRGB555()
{
    PixelSize = 2;
    BitCountR = 5; ShiftR = 10;
    BitCountG = 5; ShiftG = 5;
    BitCountB = 5; ShiftB = 0;
    BitCountA = 0; ShiftA = 0;
}

agiColorModelRGB565::agiColorModelRGB565()
{
    PixelSize = 2;
    BitCountR = 5; ShiftR = 11;
    BitCountG = 6; ShiftG = 5;
    BitCountB = 5; ShiftB = 0;
    BitCountA = 0; ShiftA = 0;
}

agiColorModelRGB555_Rev::agiColorModelRGB555_Rev()
{
    PixelSize = 2;
    BitCountR = 5; ShiftR = 0;
    BitCountG = 5; ShiftG = 5;
    BitCountB = 5; ShiftB = 10;
    BitCountA = 0; ShiftA = 0;
}

agiColorModelRGB565_Rev::agiColorModelRGB565_Rev()
{
    PixelSize = 2;
    BitCountR = 5; ShiftR = 0;
    BitCountG = 6; ShiftG = 5;
    BitCountB = 5; ShiftB = 11;
    BitCountA = 0; ShiftA = 0;
}

agiColorModelRGB888::agiColorModelRGB888()
{
    PixelSize = 4;
    BitCountR = 8; ShiftR = 16;
    BitCountG = 8; ShiftG = 8;
    BitCountB = 8; ShiftB = 0;
    BitCountA = 0; ShiftA = 0;
}

agiColorModelRGB888_Rev::agiColorModelRGB888_Rev()
{
    PixelSize = 4;
    BitCountR = 8; ShiftR = 0;
    BitCountG = 8; ShiftG = 8;
    BitCountB = 8; ShiftB = 16;
    BitCountA = 0; ShiftA = 0;
}

agiColorModelRGBA5551::agiColorModelRGBA5551()
{
    PixelSize = 2;
    BitCountR = 5; ShiftR = 10;
    BitCountG = 5; ShiftG = 5;
    BitCountB = 5; ShiftB = 0;
    BitCountA = 1; ShiftA = 15;
}

agiColorModelRGBA4444::agiColorModelRGBA4444()
{
    PixelSize = 2;
    BitCountR = 4; ShiftR = 8;
    BitCountG = 4; ShiftG = 4;
    BitCountB = 4; ShiftB = 0;
    BitCountA = 4; ShiftA = 12;
}

agiColorModelARGB::agiColorModelARGB()
{
    PixelSize = 4;
    BitCountR = 8; ShiftR = 16;
    BitCountG = 8; ShiftG = 8;
    BitCountB = 8; ShiftB = 0;
    BitCountA = 8; ShiftA = 24;
}

agiColorModelABGR::agiColorModelABGR()
{
    PixelSize = 4;
    BitCountR = 8; ShiftR = 0;
    BitCountG = 8; ShiftG = 8;
    BitCountB = 8; ShiftB = 16;
    BitCountA = 8; ShiftA = 24;
}

// All virtual function implementations (GetColor, FindColor, Filter, SetPixel)
#define IMPLEMENT_COLOR_MODEL_32BIT(cls) \
u32 cls::GetColor(agiRgba color) \
{ return PackColor(color.R, color.G, color.B, color.A, ShiftR, ShiftG, ShiftB, ShiftA, BitCountR, BitCountG, BitCountB, BitCountA); } \
u32 cls::FindColor(agiRgba color) \
{ return GetColor(color); } \
u32 cls::Filter(u32, u32, u32, u32) \
{ return 0; } \
void cls::SetPixel(agiSurfaceDesc* surface, i32 x, i32 y, u32 color) \
{ WritePixel32(surface->Surface, surface->Pitch, x, y, color); }

#define IMPLEMENT_COLOR_MODEL_16BIT(cls) \
u32 cls::GetColor(agiRgba color) \
{ return PackColor(color.R, color.G, color.B, color.A, ShiftR, ShiftG, ShiftB, ShiftA, BitCountR, BitCountG, BitCountB, BitCountA); } \
u32 cls::FindColor(agiRgba color) \
{ return GetColor(color); } \
u32 cls::Filter(u32, u32, u32, u32) \
{ return 0; } \
void cls::SetPixel(agiSurfaceDesc* surface, i32 x, i32 y, u32 color) \
{ WritePixel16(surface->Surface, surface->Pitch, x, y, color); }

IMPLEMENT_COLOR_MODEL_32BIT(agiColorModelRGB888)
IMPLEMENT_COLOR_MODEL_32BIT(agiColorModelRGB888_Rev)
IMPLEMENT_COLOR_MODEL_32BIT(agiColorModelARGB)
IMPLEMENT_COLOR_MODEL_32BIT(agiColorModelABGR)
IMPLEMENT_COLOR_MODEL_16BIT(agiColorModelRGB555)
IMPLEMENT_COLOR_MODEL_16BIT(agiColorModelRGB565)
IMPLEMENT_COLOR_MODEL_16BIT(agiColorModelRGB555_Rev)
IMPLEMENT_COLOR_MODEL_16BIT(agiColorModelRGB565_Rev)
IMPLEMENT_COLOR_MODEL_16BIT(agiColorModelRGBA5551)
IMPLEMENT_COLOR_MODEL_16BIT(agiColorModelRGBA4444)

agiColorModelRGB555::~agiColorModelRGB555() = default;
agiColorModelRGB565::~agiColorModelRGB565() = default;
agiColorModelRGB555_Rev::~agiColorModelRGB555_Rev() = default;
agiColorModelRGB565_Rev::~agiColorModelRGB565_Rev() = default;
agiColorModelRGB888::~agiColorModelRGB888() = default;
agiColorModelRGB888_Rev::~agiColorModelRGB888_Rev() = default;
agiColorModelRGBA5551::~agiColorModelRGBA5551() = default;
agiColorModelRGBA4444::~agiColorModelRGBA4444() = default;
agiColorModelARGB::~agiColorModelARGB() = default;
agiColorModelABGR::~agiColorModelABGR() = default;

RcOwner<agiColorModel> agiColorModel::FindMatch(agiSurfaceDesc* surface)
{
    return FindMatch(surface->PixelFormat.RBitMask, surface->PixelFormat.GBitMask, surface->PixelFormat.BBitMask,
        surface->PixelFormat.RGBAlphaBitMask);
}

static agiColorModel* const ColorModels[] {
    &ColorModelRGB888,
    &ColorModelRGB888_Rev,
    &ColorModelRGB555,
    &ColorModelRGB565,
    &ColorModelRGB555_Rev,
    &ColorModelRGB565_Rev,
    &ColorModelRGBA5551,
    &ColorModelRGBA4444,
    &ColorModelARGB,
    &ColorModelABGR,
    nullptr,
};

RcOwner<agiColorModel> agiColorModel::FindMatch(i32 mask_r, i32 mask_g, i32 mask_b, i32 mask_a)
{
    for (agiColorModel* const* models = ColorModels; *models; ++models)
    {
        agiColorModel* model = *models;

        if ((static_cast<u32>(mask_r) == model->GetMaskR()) && (static_cast<u32>(mask_g) == model->GetMaskG()) &&
            (static_cast<u32>(mask_b) == model->GetMaskB()) && (static_cast<u32>(mask_a) == model->GetMaskA()))
        {
            return as_owner AddRc(model);
        }
    }

    Quitf("Couldn't find match for R=%x G=%x B=%x A=%x", mask_r, mask_g, mask_b, mask_a);
}

u32 agiColorModelARGB::GetPixel(agiSurfaceDesc* surface, i32 x, i32 y)
{
    return reinterpret_cast<u32*>(static_cast<u8*>(surface->Surface) + (y * surface->Pitch))[x];
}

u32 agiColorModelRGB555::GetPixel(agiSurfaceDesc* surface, i32 x, i32 y)
{
    return reinterpret_cast<u16*>(static_cast<u8*>(surface->Surface) + (y * surface->Pitch))[x];
}

u32 agiColorModelRGB565::GetPixel(agiSurfaceDesc* surface, i32 x, i32 y)
{
    return reinterpret_cast<u16*>(static_cast<u8*>(surface->Surface) + (y * surface->Pitch))[x];
}

u32 agiColorModelRGB555_Rev::GetPixel(agiSurfaceDesc* surface, i32 x, i32 y)
{
    return reinterpret_cast<u16*>(static_cast<u8*>(surface->Surface) + (y * surface->Pitch))[x];
}

u32 agiColorModelRGB565_Rev::GetPixel(agiSurfaceDesc* surface, i32 x, i32 y)
{
    return reinterpret_cast<u16*>(static_cast<u8*>(surface->Surface) + (y * surface->Pitch))[x];
}

u32 agiColorModelRGB888::GetPixel(agiSurfaceDesc* surface, i32 x, i32 y)
{
    return reinterpret_cast<u32*>(static_cast<u8*>(surface->Surface) + (y * surface->Pitch))[x];
}

u32 agiColorModelRGB888_Rev::GetPixel(agiSurfaceDesc* surface, i32 x, i32 y)
{
    return reinterpret_cast<u32*>(static_cast<u8*>(surface->Surface) + (y * surface->Pitch))[x];
}

u32 agiColorModelRGBA5551::GetPixel(agiSurfaceDesc* surface, i32 x, i32 y)
{
    return reinterpret_cast<u16*>(static_cast<u8*>(surface->Surface) + (y * surface->Pitch))[x];
}

u32 agiColorModelRGBA4444::GetPixel(agiSurfaceDesc* surface, i32 x, i32 y)
{
    return reinterpret_cast<u16*>(static_cast<u8*>(surface->Surface) + (y * surface->Pitch))[x];
}

u32 agiColorModelABGR::GetPixel(agiSurfaceDesc* surface, i32 x, i32 y)
{
    return reinterpret_cast<u32*>(static_cast<u8*>(surface->Surface) + (y * surface->Pitch))[x];
}
