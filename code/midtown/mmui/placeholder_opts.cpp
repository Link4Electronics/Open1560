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

define_dummy_symbol(mmui_placeholder_opts);

#include "placeholder_opts.h"

#include "mmwidget/manager.h"
#include "mmwidget/navbar.h"

#include "mmeffects/mmtext.h"
#include "agi/bitmap.h"
#include "agi/pipeline.h"
#include "arts7/sim.h"
#include "arts7/cullmgr.h"

// ── AudioOptionMenu ──────────────────────────────────────────────

AudioOptionMenu::AudioOptionMenu(i32 menu_id)
    : UIMenu(menu_id)
{
    AssignName(LOC_TEXT("Audio Options"));
    AssignBackground("oaud_back");

    AddBMButton(IDC_PLACEHOLDER_DONE, "onav_done"_xconst, 0.2f, 0.9f, 4);

    SetBstate(0);
}

void AudioOptionMenu::PreSetup()
{
    prev_menu_id_ = IDM_OPTIONS;

    if (MenuMgr())
        MenuMgr()->GetNavBar()->TurnOffPrev();
}

void AudioOptionMenu::PostSetup()
{
    if (MenuMgr())
        MenuMgr()->GetNavBar()->TurnOnPrev();
}

// ── GraphicsOptionMenu ───────────────────────────────────────────

GraphicsOptionMenu::GraphicsOptionMenu(i32 menu_id)
    : UIMenu(menu_id)
{
    AssignName(LOC_TEXT("Graphics Options"));
    AssignBackground("ogra_back");

    AddBMButton(IDC_PLACEHOLDER_DONE, "onav_done"_xconst, 0.2f, 0.9f, 4);

    SetBstate(0);
}

void GraphicsOptionMenu::PreSetup()
{
    prev_menu_id_ = IDM_OPTIONS;

    if (MenuMgr())
        MenuMgr()->GetNavBar()->TurnOffPrev();
}

void GraphicsOptionMenu::PostSetup()
{
    if (MenuMgr())
        MenuMgr()->GetNavBar()->TurnOnPrev();
}

// ── ControlOptionMenu ────────────────────────────────────────────

ControlOptionMenu::ControlOptionMenu(i32 menu_id)
    : UIMenu(menu_id)
{
    AssignName(LOC_TEXT("Controls Options"));
    AssignBackground("ocon_back");

    AddBMButton(IDC_PLACEHOLDER_DONE, "onav_done"_xconst, 0.2f, 0.9f, 4);

    SetBstate(0);
}

void ControlOptionMenu::PreSetup()
{
    prev_menu_id_ = IDM_OPTIONS;

    if (MenuMgr())
        MenuMgr()->GetNavBar()->TurnOffPrev();
}

void ControlOptionMenu::PostSetup()
{
    if (MenuMgr())
        MenuMgr()->GetNavBar()->TurnOnPrev();
}

// ── AboutOptionMenu ──────────────────────────────────────────────

AboutOptionMenu::AboutOptionMenu(i32 menu_id)
    : UIMenu(menu_id)
{
    AssignName(LOC_TEXT("About"));
    AssignBackground("about_back");

    AddBMButton(IDC_PLACEHOLDER_DONE, "onav_done"_xconst, 0.2f, 0.9f, 4);

    // Create product ID text node (original: menu-local x=0.203125, y=0.2708333, w=0.15625, h=0.0375
    // → screen ~0.27, 0.30, 0.12, 0.032 with default menu margins)
    Ptr<mmTextNode> text_node = arnew mmTextNode();
    text_node->Init(0.27f, 0.30f, 0.12f, 0.032f, 1, BITMAP_TRANSPARENT);
    text_node->AddText(MenuMgr()->GetFont(20), LOC_TEXT("Open1560"), MM_TEXT_CENTER, 0, 0);
    AdoptChild(as_owner std::move(text_node));

    // Load scrolling credits image. Try "ABOUT_CRED" first (from ui.ar), fall back to "credits" (from 1560.ar).
    credits_ = as_rc Pipe()->GetBitmap("ABOUT_CRED", 0.0f, 0.0f, 0);
    if (!credits_)
        credits_ = as_rc Pipe()->GetBitmap("CREDITS", 0.0f, 0.0f, 0);

    if (credits_)
    {
        credits_width_ = credits_->GetWidth();
        credits_height_ = credits_->GetHeight();

        // Position: hotspot at menu-local (0.1, 0.1, 0.5, 0.5) → screen-normalized after ScaleWidget
        // x = menu_x_ + 0.1 * menu_width_ = 0.114 + 0.0775 = 0.1915
        // y = menu_y_ + 0.1 * menu_height_ = 0.07 + 0.0855 = 0.1555
        // h = 0.5 * menu_height_ = 0.4275
        credits_x_ = static_cast<i32>(0.1915f * Pipe()->GetWidth());
        credits_y_ = static_cast<i32>(0.1555f * Pipe()->GetHeight());
        credits_vheight_ = static_cast<i32>(0.4275f * Pipe()->GetHeight());
    }

    SetBstate(0);
}

void AboutOptionMenu::PreSetup()
{
    prev_menu_id_ = IDM_OPTIONS;

    if (MenuMgr())
        MenuMgr()->DisableNavBar();

    credits_scroll_ = 0;
    credits_start_ = Sim()->GetElapsed();
}

void AboutOptionMenu::PostSetup()
{
    if (MenuMgr())
        MenuMgr()->EnableNavBar();
}

void AboutOptionMenu::Update()
{
    if (credits_)
    {
        f32 elapsed = Sim()->GetElapsed();
        f32 dt = elapsed - credits_start_;

        // Wait 1.5s before starting scroll, then scroll at 50 px/s
        if (dt > 1.5f && credits_height_ > 0)
        {
            f32 scroll = 50.0f * dt - 74.5f;
            i32 raw = static_cast<i32>(scroll);
            credits_scroll_ = raw % credits_height_;
            if (credits_scroll_ < 0)
                credits_scroll_ += credits_height_;
        }

        CullMgr()->DeclareBitmap(this, credits_.get());
    }

    UIMenu::Update();
}

void AboutOptionMenu::Cull()
{
    if (!credits_)
        return;

    i32 scroll = credits_scroll_;
    i32 vis_h = credits_vheight_;
    i32 src_h = credits_height_;

    // Scroll DOWN: start from the top of the image and move downward.
    // Use src_y = height - scroll - vis_h, which decreases as scroll increases.
    i32 src_y = src_h - scroll - vis_h;

    if (src_y < 0)
    {
        // Wrapped past the bottom — render in two parts
        i32 h1 = -src_y;             // rows from bottom of image to show
        i32 h2 = vis_h - h1;         // remaining rows from top of image

        Pipe()->CopyBitmap(credits_x_, credits_y_, credits_.get(), 0, src_h - h1, credits_width_, h1);
        Pipe()->CopyBitmap(credits_x_, credits_y_ + h1, credits_.get(), 0, 0, credits_width_, h2);
    }
    else
    {
        Pipe()->CopyBitmap(credits_x_, credits_y_, credits_.get(), 0, src_y, credits_width_, vis_h);
    }
}
