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

define_dummy_symbol(mmeffects_vehform);

#include "vehform.h"

#include "agi/texdef.h"
#include "agiworld/getmesh.h"
#include "agiworld/quality.h"
#include "agiworld/texsheet.h"
#include "agiworld/texsort.h"
#include "arts7/cullmgr.h"
#include "mmcity/cullcity.h"
#include "stream/fsystem.h"

static mem::cmd_param PARAM_menu_refl {"menurefl"};

mmVehicleForm::mmVehicleForm()
    : color_pointer(&color_index_)
{
    if (SphMapTex)
    {
        SphMapTex->AddRef();
    }
    else
    {
        if (agiRQ.SphMap && PARAM_menu_refl.get_or<bool>(true))
        {
            t_mmEnvSetup* env = &mmEnvSetup[1][0];

            SphMapTex = as_raw GetPackedTexture(xconst(env->SphereMap), 0);

            if (SphMapTex)
                SphMapTex->Tex.Props |= agiTexProp::AlphaGlow;
        }
    }
}

mmVehicleForm::~mmVehicleForm()
{
    if (SphMapTex)
    {
        if (SphMapTex->Release() == 0)
            SphMapTex = nullptr;
    }
}

void mmVehicleForm::Update()
{
    if (vehicle_mesh_ && shadow_mesh_)
    {
        CullMgr()->DeclareCullable(this);
    }
}

void (*mmVehicleForm::Lighter)(u8*, u32*, u32*, agiMeshSet*) {};

void mmVehicleForm::SetShape(char* name, char* group, char* arg3, Vector3* offset)
{
    // Load per-vehicle texture sheet (VPBUG.TSH, etc.) so mesh textures resolve
    // TEXSHEET.Load() safely handles non-existent files and duplicate loads
    char tsh_path[64];
    arts_sprintf(tsh_path, "mtl/%s.TSH", name);
    TEXSHEET.Load(tsh_path);

    vehicle_mesh_ = GetMeshSet(name, group, offset, 0x107);

    if (arg3)
        shadow_mesh_ = GetMeshSet(name, arg3, offset, 0x107);
}

void mmVehicleForm::Cull()
{
    if (vehicle_mesh_ && *color_pointer >= 0)
    {
        u32 color = static_cast<u32>(*color_pointer);
        vehicle_mesh_->DrawColor(color, MESH_DRAW_CLIP);
    }

    if (shadow_mesh_)
    {
        Vector4 plane(0.0f, 1.0f, 0.0f, 0.0f);
        Vector3 light_dir(0.0f, -1.0f, 0.0f);
        shadow_mesh_->DrawShadow(MESH_DRAW_CLIP, plane, light_dir);
    }
}
