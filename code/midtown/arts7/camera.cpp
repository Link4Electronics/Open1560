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

define_dummy_symbol(arts7_camera);

#include "camera.h"

#include "agi/bitmap.h"
#include "agi/lmodel.h"
#include "agi/pipeline.h"
#include "agi/rsys.h"
#include "agi/surface.h"
#include "agi/viewport.h"
#include "data7/printer.h"
#include "sim.h"

#include <cmath>

#include <unistd.h>

void asCamera::SetView(f32 horz_fov, f32 aspect, f32 near_clip, f32 far_clip)
{
    f32 vert_tan = std::tan(horz_fov / 2.0f);

    if (aspect != 0.0f)
    {
        vert_tan /= aspect;
        horz_fov = 2.0f * std::atan(vert_tan * aspect);

        // Use auto aspect
        aspect = 0.0f;
    }

    fov_ = horz_fov;
    near_clip_ = near_clip;
    far_clip_ = far_clip;

    if (aspect == 0.0f)
    {
        auto_aspect_ = true;
        aspect_ = 1.0f;
    }
    else
    {
        // auto_aspect_ = false;
        aspect_ = aspect;
    }
}

void asCamera::DrawBegin()
{
    write(2, "DBG Camera::DrawBegin\n", 22);

    i32 draw_mode = Sim()->GetDrawMode();

    if (draw_mode == agiDrawTextured)
        draw_mode = draw_mode_;

    agiCurState.SetDrawMode(static_cast<agiDrawMode>(draw_mode));

    if (fog_density_ != 0.0f)
    {
        agiCurState.SetFogMode(agiFogMode::Pixel);
        agiCurState.SetFogStart(fog_start_);
        agiCurState.SetFogEnd(fog_end_);
        agiCurState.SetFogDensity(fog_density_);
        agiCurState.SetFogColor((static_cast<u32>(fog_color_.x * 255.0) << 16) |
            (static_cast<u32>(fog_color_.y * 255.0) << 8) | static_cast<u32>(fog_color_.z * 255.0));
    }
    else
    {
        agiCurState.SetFogMode(agiFogMode::None);
    }

    viewport_->Activate();

    if (underlay_bitmap_)
    {
        write(2, "DBG Camera: has underlay\n", 25);

        if (underlay_callback_)
        {
            write(2, "DBG Camera: has callback\n", 25);
            underlay_callback_->Call();
        }
        else
        {
            write(2, "DBG Camera: about to CopyBitmap\n", 32);
            Pipe()->CopyBitmap(
                UI_XPos, UI_YPos, underlay_bitmap_, 0, 0, underlay_bitmap_->GetWidth(), underlay_bitmap_->GetHeight());
            write(2, "DBG Camera: after CopyBitmap\n", 29);
        }

        write(2, "DBG Camera: before Is3D check\n", 30);
        if (!underlay_bitmap_->Is3D())
        {
            write(2, "DBG Camera: calling BeginScene\n", 31);
            Pipe()->BeginScene();
        }
        write(2, "DBG Camera: underlay done\n", 26);
    }
    else
    {
        write(2, "DBG Camera: no underlay\n", 24);
    }

    i32 clear_flags = clear_flags_;

    if (draw_mode < agiDrawSolid || Sim()->IsDebugDrawEnabled())
    {
        clear_flags |= AGI_VIEW_CLEAR_TARGET;
    }

    if (underlay_bitmap_ && !underlay_callback_ && agiCurState.GetDrawMode() != agiDrawDepth)
    {
        clear_flags &= ~AGI_VIEW_CLEAR_TARGET;
    }

    viewport_->Clear(clear_flags);

    if (light_model_)
        light_model_->Activate();
}

asCamera::asCamera()
{
    Rc<agiViewport> vp = as_rc Pipe()->CreateViewport();
    viewport_ = vp.release();
    clear_flags_ = AGI_VIEW_CLEAR_TARGET | AGI_VIEW_CLEAR_ZBUFFER;
    draw_mode_ = agiDrawTextured;

    SetView(1.0f, 1.0f, 0.1f, 1000.0f);
    SetViewport(0.0f, 0.0f, 1.0f, 1.0f, 0);
}

void asCamera::SetUnderlay(aconst char* path)
{
    if (path && *path)
    {
        Rc<agiBitmap> bm = as_rc Pipe()->GetBitmap(path, 1.0f, 1.0f, 0);
        if (bm)
            underlay_bitmap_ = bm.release();
    }
    else
    {
        underlay_bitmap_ = nullptr;
    }
}

void asCamera::SetViewport(f32 x, f32 y, f32 w, f32 h, i32 /*arg5*/)
{
    viewport_->GetParams().X = x;
    viewport_->GetParams().Y = y;
    viewport_->GetParams().Width = w;
    viewport_->GetParams().Height = h;
}

asCamera::~asCamera() = default;

void asCamera::Update()
{
    asNode::Update();
}

MetaClass* asCamera::GetClass()
{
    return asNode::GetClass();
}

#ifdef ARTS_DEV_BUILD
void asCamera::AddWidgets(Bank* /*arg1*/)
{}
#endif
