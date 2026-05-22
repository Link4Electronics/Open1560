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

define_dummy_symbol(mmwidget_menu);

#include "menu.h"

#include "agi/pipeline.h"
#include "arts7/camera.h"
#include "arts7/midgets.h"
#include "eventq7/keys.h"
#include "pcwindis/dxinit.h"

#include "manager.h"
#include "textfield.h"
#include "widget.h"

#include "bm_button.h"

#include <algorithm>

#include <SDL3/SDL_keyboard.h>

UIMenu::UIMenu(i32 menu_id)
{
    action_source_ = 0;
    state_ = 0;
    enabled_ = 0;
    menu_id_ = menu_id;
    field_30 = 0;
    widget_count_ = 0;
    field_38 = 0;
    field_44 = nullptr;
    field_48 = 0;
    menu_x_ = 0.114f;
    menu_y_ = 0.07f;
    menu_width_ = 0.775f;
    menu_height_ = 0.855f;
    field_5C = 0.0f;
    field_60 = 0.0f;
    field_64 = 0.0f;
    field_68 = 0;
    p_b_state_ = &b_state_;
    b_state_ = 0;
    field_74 = 0;
    widget_id_ = 0;
    prev_menu_id_ = -1;
    focus_widget_index_ = 0;
    field_84 = 0.0f;
    field_88 = 0;
}

void UIMenu::PreSetup()
{
    if (!MenuMgr()->Is3D())
    {
        if (asCamera* camera = MenuMgr()->GetCamera())
            camera->SetViewport(0.0f, 0.0f, 1.0f, 1.0f, 1);
    }
}

void UIMenu::PostSetup()
{}

void UIMenu::BackUp()
{
    state_ = MENU_STATE_1;
}

void UIMenu::CheckInput()
{
    // Ignore any events while midgets are open
    if (MIDGETSPTR->IsOpen())
    {
        MenuMgr()->GetEventQ()->Clear();
        return;
    }

    eqEvent event;

    while (MenuMgr()->GetEventQ()->Pop(&event))
    {
        if (event.Type != eqEventType::Keyboard)
        {
            if (event.Type == eqEventType::Mouse)
                MenuMgr()->ToggleWidgetSnapping(false);

            continue;
        }

        eqKeyboardEvent& kev = event.Key;

        switch (kev.Key) // Enable widget snapping when using arrows or joystick
        {
            case EQ_VK_TAB:
            case EQ_VK_LEFT:
            case EQ_VK_RIGHT:
            case EQ_VK_UP:
            case EQ_VK_DOWN:
            case EQ_VK_GAMEPAD_A:
            case EQ_VK_GAMEPAD_B:
            case EQ_VK_GAMEPAD_DPAD_LEFT:
            case EQ_VK_GAMEPAD_DPAD_RIGHT:
            case EQ_VK_GAMEPAD_DPAD_UP:
            case EQ_VK_GAMEPAD_DPAD_DOWN:
            case EQ_VK_GAMEPAD_VIEW: MenuMgr()->ToggleWidgetSnapping(true); break;
        }

        switch (kev.Key) // Translate gamepad buttons to keyboard buttons
        {
            case EQ_VK_GAMEPAD_A: kev.Key = EQ_VK_RETURN; break;
            case EQ_VK_GAMEPAD_B: kev.Key = EQ_VK_ESCAPE; break;
            case EQ_VK_GAMEPAD_DPAD_LEFT: kev.Key = EQ_VK_LEFT; break;
            case EQ_VK_GAMEPAD_DPAD_RIGHT: kev.Key = EQ_VK_RIGHT; break;
            case EQ_VK_GAMEPAD_DPAD_UP: kev.Key = EQ_VK_UP; break;
            case EQ_VK_GAMEPAD_DPAD_DOWN: kev.Key = EQ_VK_DOWN; break;
        }

        if (ScanInput(&event))
        {
            if (!kev.IsMouseButton())
                KeyboardAction(event);
        }
        else
        {
            if (kev.Key == EQ_VK_ESCAPE)
                ClearAction();
        }
    }
}

void UIMenu::AddWidget(uiWidget* widget, aconst char* label, f32 x, f32 y, f32 w, f32 h, i32 id, aconst char* icon)
{
    widget->X = x;
    widget->Y = y;
    widget->Width = w;
    widget->Height = h;
    widget->MinX = x;
    widget->MinY = y;
    widget->MaxX = x + w;
    widget->MaxY = y + h;
    widget->field_6C = 0;
    widget->Label = label;
    widget->Icon = icon;
    widget->Menu = this;
    widget->WidgetID = (id != -1) ? id : (widget_count_ + 1000);

    Ptr<uiWidget*[]> widgets = arnewa uiWidget* [widget_count_ + 1] {};
    for (i32 i = 0; i < widget_count_; ++i)
        widgets[i] = widgets_[i];
    widgets[widget_count_] = widget;
    widgets_ = std::move(widgets);
    ++widget_count_;

    // Try and maintain a top-to-bottom order so widgets are navigated in a sane way
    // TODO: Add a function to override the ordering
    std::sort(&widgets_[0], &widgets_[widget_count_], [](uiWidget* lhs, uiWidget* rhs) {
        if (f32 delta = lhs->Y - rhs->Y; std::abs(delta) > 0.005f)
            return delta < 0.0f;
        return lhs->X < rhs->X;
    });

    if (label && *label)
        widget->AddToolTip(this, LOC_TEXT(label));
}

void UIMenu::AssignBackground(aconst char* background_name)
{
    background_name_ = background_name;
}

void UIMenu::AssignName(LocString* name)
{
    menu_name_ = name->Text;
}

void UIMenu::ClearAction()
{
    state_ = MENU_STATE_2;
}

void UIMenu::ClearSelected()
{
    if (widget_count_ > 0 && !MenuMgr()->HasFocusedWidget())
        GetActiveWidget()->Switch(false);
}

void UIMenu::GetDimensions(f32& x, f32& y, f32& w, f32& h)
{
    x = menu_x_;
    y = menu_y_;
    w = menu_width_;
    h = menu_height_;
}

b32 UIMenu::Increment()
{
    DisableIME();

    if (FindTheNextFocusWidget() != -1)
        return false;

    if (MenuMgr()->GetDialogMenu() || MenuMgr()->Is3D())
    {
        FindTheFirstFocusWidget();
        return false;
    }

    return true;
}

void UIMenu::KeyboardAction(eqEvent event)
{
    if (!IsNodeActive())
        return;

    if (MenuMgr()->HasFocusedWidget())
    {
        MenuMgr()->GetFocusedWidget()->CaptureAction(event);
        MenuMgr()->GetActiveWidget()->MouseHit = false;
    }
    else
    {
        GetActiveWidget()->Action(event);

        if (event.Key.Key == EQ_VK_RETURN)
            widget_id_ = GetActiveWidget()->WidgetID;
    }
}

void UIMenu::MouseAction(eqEvent event)
{
    if (GetActiveWidget() != MenuMgr()->GetActiveImeField())
    {
        DisableIME();
        MenuMgr()->SetActiveImeField(nullptr);
    }

    GetActiveWidget()->Action(event);
    GetActiveWidget()->ResetToolTip();
}

void UIMenu::ClearToolTip()
{
    for (i32 i = 0; i < widget_count_; ++i)
        widgets_[i]->ResetToolTip();
}

void UIMenu::ClearWidgets()
{
    for (i32 i = 0; i < widget_count_; ++i)
    {
        uiWidget* widget = widgets_[i];

        if (widget->Active)
            widget->Switch(false);
    }
}

b32 UIMenu::Decrement()
{
    DisableIME();

    if (FindThePrevFocusWidget() != -1)
        return false;

    if (MenuMgr()->GetDialogMenu() || MenuMgr()->Is3D())
    {
        FindTheLastFocusWidget();
        return false;
    }

    return true;
}

void UIMenu::Disable()
{
    DeactivateNode();
    enabled_ = false;
    state_ = MENU_STATE_3;
    PostSetup();
}

void UIMenu::ScaleWidget(f32& x, f32& y, f32& w, f32& h)
{
    f32 l = menu_x_ + x * menu_width_;
    f32 b = menu_y_ + y * menu_height_;
    f32 r = l + (w * menu_width_);
    f32 t = b + (h * menu_height_);

    Pipe()->RoundNormalized(l, b);
    Pipe()->RoundNormalized(r, t);

    x = l;
    y = b;
    w = r - x;
    h = t - y;
}

b32 UIMenu::ScanInput(eqEvent* event)
{
    // Returns true if the menu was active, and the event was for a key press

    if (!IsNodeActive() || (event->Type != eqEventType::Keyboard) || !(event->Key.Modifiers & EQ_KMOD_DOWN))
        return false;

    // TODO: return result of ScanGlobalKeys?
    MenuMgr()->ScanGlobalKeys(event->Key.Key);

    return true;
}

void UIMenu::SetNavigationOrder(const char* const* labels, usize count)
{
    const auto order = [labels, end = labels + count](uiWidget* widget) {
        auto find =
            std::find_if(labels, end, [widget](const char* label) { return !std::strcmp(label, widget->Label); });
        ArAssert(find != end, "Unexpected widget");
        return find - end;
    };

    std::sort(
        &widgets_[0], &widgets_[widget_count_], [&](uiWidget* lhs, uiWidget* rhs) { return order(lhs) < order(rhs); });
}

uiWidget* UIMenu::FindWidget(i32 id)
{
    for (i32 i = 0; i < widget_count_; ++i)
    {
        uiWidget* widget = widgets_[i];

        if (widget->WidgetID == id)
            return widget;
    }

    return nullptr;
}

i32 UIMenu::FindFocusWidget(i32 start, i32 step)
{
    if (uiWidget* active = GetActiveWidget())
        active->Switch(false);

    for (i32 i = start; (i >= 0) && (i < widget_count_); i += step)
    {
        if (uiWidget* widget = widgets_[i]; widget->Enabled && !widget->ReadOnly)
        {
            *p_b_state_ = i;
            widgets_[i]->Switch(true);
            return i;
        }
    }

    return -1;
}

void UIMenu::DisableIME()
{
    if (SDL_TextInputActive(g_MainWindow))
    {
        Displayf("Disabled Text Input");
        SDL_StopTextInput(g_MainWindow);
    }
}

void UIMenu::Enable()
{
    ActivateNode();
    enabled_ = true;
    field_74 = 1;
    field_88 = 0;
    state_ = MENU_STATE_2;
    PreSetup();
    SetBstate(focus_widget_index_);

    if (widget_count_ && !MenuMgr()->Is3D())
    {
        GetWidget(focus_widget_index_)->Update();
        GetWidget(focus_widget_index_)->Switch(true);
    }
}

i32 UIMenu::FindTheFirstFocusWidget()
{
    return FindFocusWidget(0, 1);
}

i32 UIMenu::FindTheNextFocusWidget()
{
    i32 active = *p_b_state_;
    return FindFocusWidget(active + (GetWidget(active)->Active ? 1 : 0), 1);
}

i32 UIMenu::FindThePrevFocusWidget()
{
    i32 active = *p_b_state_;
    return FindFocusWidget(active - (GetWidget(active)->Active ? 1 : 0), -1);
}

i32 UIMenu::FindTheLastFocusWidget()
{
    return FindFocusWidget(widget_count_ - 1, -1);
}

void UIMenu::Update()
{
    asNode::Update();

    for (i32 i = 0; i < widget_count_; ++i)
        widgets_[i]->Update();
}

void UIMenu::SetBstate(i32 index)
{
    if (index >= 0 && index < widget_count_)
    {
        *p_b_state_ = index;
        GetWidget(index)->Switch(true);
    }
}

UIBMButton* UIMenu::AddBMButton(i32 idc, aconst char* name, f32 x, f32 y, i32 type, Callback cb_1, i32* arg7,
    i32 arg8, i32 arg9, Callback arg10)
{
    Ptr<UIBMButton> button = arnew UIBMButton();
    button->Init(const_cast<char*>(name), x, y, type, 0, arg7, arg8, arg9, nullptr, cb_1, arg10);
    AddWidget(button.get(), name, x, y, 0.0f, 0.0f, idc, nullptr);
    return button.release();
}
