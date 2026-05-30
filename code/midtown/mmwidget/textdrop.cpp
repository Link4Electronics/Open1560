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
#include "data7/callback.h"
#include "data7/str.h"
#include "eventq7/eventq.h"
#include "localize/localize.h"
#include "mmeffects/mmtext.h"
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

    // Set up the TextDropWidget with options
    if (DropWidget)
    {
        DropWidget->Init(nullptr, nullptr, arg3, arg4, arg5, arg6, arg6, std::move(arg7), arg8);
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

void UITextDropdown::Action(eqEvent /*arg1*/)
{
    if (!DropWidget || DropWidget->DisabledMask)
        return;

    DropWidget->IncDrop();

    if (ValuePtr)
        *ValuePtr = DropWidget->CurrentValue;

    UpdateValueText();
}

void UITextDropdown::AssignString(string options)
{
    if (DropWidget)
    {
        DropWidget->SetString(std::move(options));

        if (ValuePtr)
            SetValue(*ValuePtr);
    }
}

void UITextDropdown::CaptureAction(eqEvent /*arg1*/)
{}

void UITextDropdown::Cull()
{
    if (!IsNodeActive())
        return;
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

void UITextDropdown::SetSliderFocus(i32 /*arg1*/)
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

void UITextDropdown::Switch(b32 arg1)
{
    if (arg1)
        ActivateNode();
    else
        DeactivateNode();
}

void UITextDropdown::Update()
{
    uiWidget::Update();
}
