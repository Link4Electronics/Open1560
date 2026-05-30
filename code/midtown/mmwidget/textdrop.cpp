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

define_dummy_symbol(mmwidget_textdrop);

#include "textdrop.h"

#include "agi/bitmap.h"
#include "agi/pipeline.h"
#include "arts7/cullmgr.h"
#include "data7/callback.h"
#include "data7/str.h"
#include "eventq7/eventq.h"
#include "localize/localize.h"
#include "mmeffects/mmtext.h"
#include "mmwidget/dropdown.h"
#include "mmwidget/icon.h"
#include "mmwidget/manager.h"
#include "mmwidget/tdwidget.h"

UITextDropdown::UITextDropdown()
{
    Ptr<TextDropWidget> widget = arnew TextDropWidget();
    DropWidget = widget.get();
    AdoptChild(Ptr<asNode>(std::move(widget)));
}

UITextDropdown::~UITextDropdown() = default;

void UITextDropdown::Init(LocString* arg1, i32* arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, string arg7, i32 arg8,
    i32 arg9, i32 arg10, Callback arg11, char* arg12)
{
    ValuePtr = arg2;

    if (ValuePtr)
        StoredValue = *ValuePtr;

    TextNode = nullptr;
    ValueNode = nullptr;
    ArrowIcon = nullptr;
    FontSize = arg8;

    // Load drop_arrow bitmap as background box (3 frames: normal, hover, expanded)
    bitmap_ = as_rc Pipe()->GetBitmap("drop_arrow", 0.0f, 0.0f, 1);

    if (bitmap_)
    {
        bitmap_width_ = bitmap_->GetWidth();
        i32 bitmap_height = bitmap_->GetHeight();
        frame_height_ = (bitmap_height + 2) / 3; // ceil(bitmap_height/3)

        // Compute pixel position from widget coords
        dst_x_ = static_cast<i32>(Pipe()->GetWidth() * arg3);
        dst_y_ = static_cast<i32>(Pipe()->GetHeight() * arg4);
    }

    // Set up the TextDropWidget with options
    if (DropWidget)
    {
        string dd_options(arg7.get());
        DropWidget->Init(nullptr, nullptr, arg3, arg4, arg5, arg6, arg6, std::move(dd_options), arg8);
    }

    // Create mmDropDown for expanded list
    {
        string dd_options(arg7.get());
        Ptr<mmDropDown> dd = arnew mmDropDown();
        dd->Init(nullptr, MenuMgr()->GetFont(arg8), arg3, arg4, arg5, arg6, std::move(dd_options), arg8);
        dd->SetDropEnabled(false);
        drop_down_ = dd.get();
        AdoptChild(Ptr<asNode>(std::move(dd)));
    }

    // Create label text node (when arg9 > 0)
    if (arg9 > 0)
    {
        Ptr<mmTextNode> label = arnew mmTextNode();
        label->Init(arg3, arg4, arg5, arg6, 1, BITMAP_TRANSPARENT);

        if (arg1)
        {
            void* font = MenuMgr()->GetFont(arg8);
            label->AddText(font, arg1, MM_TEXT_REQUIRED, 0.0f, 0.0f);
        }

        TextNode = label.get();
        AdoptChild(Ptr<asNode>(std::move(label)));
    }
    else if (arg9 < 0 && arg12)
    {
        Ptr<UIIcon> icon = arnew UIIcon();
        icon->Init(arg12, arg3, arg4);
        ArrowIcon = icon.get();
        AdoptChild(Ptr<asNode>(std::move(icon)));
    }

    // Create value text node for current option display
    {
        Ptr<mmTextNode> val = arnew mmTextNode();
        val->Init(arg3 + 0.01f, arg4 + 0.04f, arg5, arg6, 1, BITMAP_TRANSPARENT);

        void* font = MenuMgr()->GetFont(arg8);
        val->AddText(font, LOC_TEXT(""), 0, 0.0f, 0.0f);

        ValueNode = val.get();
        AdoptChild(Ptr<asNode>(std::move(val)));
    }

    MinX = arg3;
    MinY = arg4;
    MaxX = arg3 + arg5;
    MaxY = arg4 + arg6;

    // Set value from pointer
    if (ValuePtr)
        SetValue(*ValuePtr);

    Switch(true);
}

void UITextDropdown::Action(eqEvent event)
{
    if (!DropWidget || DropWidget->DisabledMask)
        return;

    if (event.Type == eqEventType::Mouse)
    {
        f32 my = event.Mouse.MouseY;

        if (expanded_)
        {
            // Click below button is on the expanded list
            f32 list_top = MinY + Height;
            if (my >= list_top && drop_down_)
            {
                f32 rel_y = (my - drop_down_->GetDropBottom()) / drop_down_->GetDropHeightUnit();
                i32 hit = static_cast<i32>(rel_y);

                if (hit >= 0 && hit < drop_down_->GetNumValues())
                {
                    DropWidget->SetValue(hit);

                    if (ValuePtr)
                        *ValuePtr = hit;

                    UpdateValueText();
                }
            }

            SetSliderFocus(0);
        }
        else
        {
            SetSliderFocus(1);
        }
    }
    else if (event.Type == eqEventType::Keyboard)
    {
        if (event.Key.Key == EQ_VK_DOWN || event.Key.Key == EQ_VK_RIGHT)
        {
            DropWidget->IncDrop();

            if (ValuePtr)
                *ValuePtr = DropWidget->CurrentValue;

            UpdateValueText();
        }
        else if (event.Key.Key == EQ_VK_UP || event.Key.Key == EQ_VK_LEFT)
        {
            DropWidget->DecDrop();

            if (ValuePtr)
                *ValuePtr = DropWidget->CurrentValue;

            UpdateValueText();
        }
        else if (event.Key.Key == EQ_VK_RETURN || event.Key.Key == EQ_VK_SPACE)
        {
            SetSliderFocus(expanded_ ? 0 : 1);
        }
    }
}

void UITextDropdown::AssignString(string options)
{
    if (DropWidget)
    {
        DropWidget->SetString(std::move(options));

        if (ValuePtr)
            SetValue(*ValuePtr);
    }

    if (drop_down_ && DropWidget)
    {
        string opts(DropWidget->Options.get());
        drop_down_->InitString(std::move(opts));
    }
}

void UITextDropdown::CaptureAction(eqEvent event)
{
    if (!DropWidget || DropWidget->DisabledMask)
        return;

    if (event.Type == eqEventType::Mouse && expanded_ && drop_down_)
    {
        f32 my = event.Mouse.MouseY;
        f32 rel_y = (my - drop_down_->GetDropBottom()) / drop_down_->GetDropHeightUnit();
        i32 hit = static_cast<i32>(rel_y);

        if (hit >= 0 && hit < drop_down_->GetNumValues())
        {
            drop_down_->SetHighlight(hit);
        }
    }
}

void UITextDropdown::Cull()
{
    if (!IsNodeActive())
        return;

    if (!bitmap_ || !bitmap_width_ || !frame_height_)
        return;

    // Determine which frame to render:
    // frame 0 = normal (src_y = 0)
    // frame 1 = hover/active (src_y = frame_height_) when Active or TextNode exists
    // frame 2 = expanded (src_y = frame_height_ * 2)
    i32 src_y = 0;

    if (expanded_)
    {
        src_y = frame_height_ * 2;
    }
    else if (Active || TextNode)
    {
        src_y = frame_height_;
    }

    Pipe()->CopyBitmap(dst_x_, dst_y_, bitmap_.get(), 0, src_y, bitmap_width_, frame_height_);
}

f32 UITextDropdown::GetScreenHeight()
{
    return Height;
}

void UITextDropdown::SetData(i32* arg1)
{
    ValuePtr = arg1;

    if (ValuePtr)
        StoredValue = *ValuePtr;
}

void UITextDropdown::SetDisabledMask(ilong arg1)
{
    if (DropWidget)
        DropWidget->DisabledMask = arg1;
}

void UITextDropdown::SetPos(f32 /*arg1*/, f32 /*arg2*/)
{}

void UITextDropdown::SetText(LocString* /*arg1*/)
{}

void UITextDropdown::UpdateValueText()
{
    if (!ValueNode || !DropWidget)
        return;

    i32 num_opts = DropWidget->Options.NumSubStrings();
    i32 val = DropWidget->CurrentValue;

    if (num_opts > 0 && val >= 0 && val < num_opts)
    {
        string sel = DropWidget->Options.SubString(val);
        ValueNode->SetString(0, LOC_TEXT(sel.get()));
    }
}

i32 UITextDropdown::SetValue(i32 arg1)
{
    i32 old = StoredValue;

    if (DropWidget)
        DropWidget->SetValue(arg1);

    StoredValue = arg1;

    if (ValuePtr)
        *ValuePtr = arg1;

    UpdateValueText();

    return old;
}

void UITextDropdown::SetSliderFocus(i32 focus)
{
    expanded_ = (focus != 0);

    if (DropWidget)
    {
        DropWidget->SetActive(focus);

        // Sync highlight on expand
        if (expanded_ && drop_down_)
        {
            drop_down_->SetHighlight(DropWidget->CurrentValue);
        }
    }

    if (drop_down_)
        drop_down_->SetDropEnabled(focus != 0);

    // Expand hit bounds when expanded so menu can find widget for click events
    if (expanded_ && drop_down_)
    {
        f32 expanded_h = Height + drop_down_->GetDropHeight();
        MaxY = MinY + expanded_h;
    }
    else
    {
        MaxY = MinY + Height;
    }
}

void UITextDropdown::Switch(b32 arg1)
{
    if (arg1)
        ActivateNode();
    else
        DeactivateNode();
}

void UITextDropdown::Update()
{
    asNode::Update();

    if (bitmap_)
        CullMgr()->DeclareBitmap(this, bitmap_.get());

    // Sync value from external pointer if changed
    if (ValuePtr)
    {
        i32 current = *ValuePtr;
        if (current != StoredValue)
            SetValue(current);
    }
}
