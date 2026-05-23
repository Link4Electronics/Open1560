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

define_dummy_symbol(agirend_surfrender);

#include "surfrender.h"

#include <unistd.h>

#include "agi/lmodel.h"
#include "agi/rsys.h"
#include "agiworld/meshset.h"
#include "lighter.h"
#include "projvtx.h"

agiSurfRenderer::agiSurfRenderer(agiRasterizer* rasterizer)
    : rasterizer_(rasterizer)
    , lighter_(nullptr)
{}

void agiSurfRenderer::BeginDraw(i32 flags)
{
    agiProjVtx::Init(flags & 1, lighter_, 0xFFFFFFFF);

    BeginGroup();
}

void agiSurfRenderer::BeginGroup()
{
    write(2, "DBG SurfRenderer::BeginGroup start\n", 35);
    agiSurfRenderer::VertexCount = 1;
    agiSurfRenderer::SurfaceCount = 0;
    write(2, "DBG SurfRenderer::BeginGroup after counts\n", 42);

    write(2, "DBG SurfRenderer: LMODEL=", 25);
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "%p\n", static_cast<void*>(agiLighter::LMODEL));
    write(2, buf, len);

    if (agiLighter::LMODEL)
    {
        write(2, "DBG SurfRenderer: setting lighter_\n", 35);
        if (agiLighter::LMODEL->Params.Monochromatic)
            lighter_ = &MONOLIGHTER;
        else
            lighter_ = &RGBLIGHTER;
    }
    write(2, "DBG SurfRenderer::BeginGroup end\n", 33);
}

void UpdateZTrick()
{
    if (ZTrick)
    {
        if (agiCurState.GetZFunc() == agiCmpFunc::LessEqual)
        {
            agiCurState.SetZFunc(agiCmpFunc::GreaterEqual);
            DepthScale = -0.24f;
            DepthOffset = 0.75f;
        }
        else
        {
            agiCurState.SetZFunc(agiCmpFunc::LessEqual);
            DepthScale = 0.24f;
            DepthOffset = 0.25f;
        }
    }
    else
    {
        agiCurState.SetZFunc(agiCmpFunc::LessEqual);
        DepthScale = agiMeshSet::DepthScale;
        DepthOffset = agiMeshSet::DepthOffset;
    }
}
