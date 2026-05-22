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

#pragma once

#include "arts7/node.h"

#include "data7/callback.h"
#include "data7/str.h"
#include "localize/localize.h"
#include "mmwidget/widget.h"

union eqEvent;
class mmIO;

class UIBMButton;
class UIBMLabel;
class UIButton;
class UICompositeScroll;
class UIControlWidget;
class UICWArray;
class UIIcon;
class UIIconW;
class UILabel;
class UIMexButton;
class UISlider;
class UITextDropdown;
class UITextField;
class UITextRoller2;
class UITextRoller;
class UITextScroll;
class UIToggleButton2;
class UIToggleButton;
class UIVScrollBar;

enum
{
    IDM_MAIN = 1,
    IDM_OPTIONS = 2,
    IDM_AUDIO = 3,
    IDM_GRAPHICS = 4,
    IDM_CONTROLS = 5,
    IDM_DRIVER = 6,
    IDM_RACE = 7,
    IDM_VEHICLE = 8,
    IDM_SHOWCASE = 9,
    IDM_NET_SELECT = 10,
    IDM_HOST_RACE = 11,
    IDM_NET_ARENA = 12,
    IDM_CNR = 13,
    IDD_TCP = 14,
    IDD_SERIAL = 16,
    IDD_NEWP = 17,
    IDD_RETRIEVE = 18,
    IDD_DREC = 19,
    IDD_HOFF = 20,
    IDD_CTRL = 21,
    IDD_RENV = 22,
    IDD_LOCKED = 23,
    IDD_BADPASS = 24,
    IDD_PASSWORD = 25,
    IDD_ZONE = 26,
    IDD_QUIT = 27,
    IDD_DELP = 28,
    IDD_DUPP = 29,
    IDD_LASTP = 30,
    IDD_ODEF = 31,
    IDD_XASSIGN = 32,
    IDD_PLIM = 33,
    IDM_ABOUT = 34,
    IDD_CENV = 35,
};

class UIMenu : public asNode
{
public:
    enum class eSource : i32
    {
        Keyboard,
        Mouse
    };

    // ??0UIMenu@@QAE@H@Z
    ARTS_IMPORT UIMenu(i32 menu_id);

    // ??1UIMenu@@UAE@XZ
    ~UIMenu() override = default;

    // ?PreSetup@UIMenu@@UAEXXZ
    ARTS_EXPORT virtual void PreSetup();

    // ?PostSetup@UIMenu@@UAEXXZ
    ARTS_EXPORT virtual void PostSetup();

    // ?BackUp@UIMenu@@UAEXXZ
    ARTS_EXPORT virtual void BackUp();

    // ?CheckInput@UIMenu@@UAEXXZ
    ARTS_EXPORT virtual void CheckInput();

    // ?IsAnOptionMenu@UIMenu@@UAEHXZ | inline
    ARTS_EXPORT virtual b32 IsAnOptionMenu()
    {
        return false;
    }

    // ?AddBMButton@UIMenu@@QAEPAVUIBMButton@@HPADMMHVCallback@@PAHHH1@Z
    ARTS_IMPORT UIBMButton* AddBMButton(i32 idc, aconst char* name, f32 x, f32 y, i32 type, Callback cb_1 = nullptr,
        i32* arg7 = nullptr, i32 arg8 = -1, i32 arg9 = -1, Callback arg10 = nullptr);

    // ?AddBMLabel@UIMenu@@QAEPAVUIBMLabel@@HPADPAVstring@@MMPAH@Z
    ARTS_IMPORT UIBMLabel* AddBMLabel(i32 arg1, char* arg2, string* arg3, f32 arg4, f32 arg5, i32* arg6);

    // ?AddButton@UIMenu@@QAEPAVUIButton@@HPAULocString@@MMMMHHVCallback@@@Z
    ARTS_IMPORT UIButton* AddButton(
        i32 arg1, LocString* arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, i32 arg7, i32 arg8, Callback arg9 = nullptr);

    // ?AddCWArray@UIMenu@@QAEPAVUICWArray@@HMMMMHVCallback@@@Z
    ARTS_IMPORT UICWArray* AddCWArray(i32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, i32 arg6, Callback arg7);

    // ?AddCompScroll@UIMenu@@QAEPAVUICompositeScroll@@HMMMMMHPAHHHVCallback@@@Z
    ARTS_IMPORT UICompositeScroll* AddCompScroll(i32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, i32 arg7,
        i32* arg8, i32 arg9, i32 arg10, Callback arg11);

    // ?AddHotSpot@UIMenu@@QAEPAVuiWidget@@HPADMMMMVCallback@@@Z
    ARTS_IMPORT uiWidget* AddHotSpot(
        i32 arg1, aconst char* arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, Callback arg7 = nullptr);

    // ?AddIconW@UIMenu@@QAEPAVUIIconW@@HPAULocString@@PADMMMMVCallback@@@Z | unused
    ARTS_IMPORT UIIconW* AddIconW(
        i32 arg1, LocString* arg2, char* arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7, Callback arg8);

    // ?AddLabel@UIMenu@@QAEPAVUILabel@@HPAULocString@@MMMMHH@Z
    ARTS_IMPORT UILabel* AddLabel(i32 id, LocString* text, f32 x, f32 y, f32 w, f32 h, i32 flags, i32 font_size);

    // ?AddMex@UIMenu@@QAEPAVUIMexButton@@HPAULocString@@PAHHMMMMHHVCallback@@@Z | unused
    ARTS_IMPORT UIMexButton* AddMex(i32 arg1, LocString* arg2, i32* arg3, i32 arg4, f32 arg5, f32 arg6, f32 arg7,
        f32 arg8, i32 arg9, i32 arg10, Callback arg11);

    // ?AddSlider@UIMenu@@QAEPAVUISlider@@HPAULocString@@PAMMMMMMMHHHHVCallback@@@Z
    ARTS_IMPORT UISlider* AddSlider(i32 arg1, LocString* arg2, f32* arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7,
        f32 arg8, f32 arg9, i32 arg10, i32 arg11, i32 arg12, i32 arg13, Callback arg14);

    // ?AddTextDropdown@UIMenu@@QAEPAVUITextDropdown@@HPAULocString@@PAHMMMMVstring@@HHHVCallback@@PAD@Z
    ARTS_IMPORT UITextDropdown* AddTextDropdown(i32 arg1, LocString* arg2, i32* arg3, f32 arg4, f32 arg5, f32 arg6,
        f32 arg7, string arg8, i32 arg9, i32 arg10, i32 arg11, Callback arg12, char* arg13);

    // ?AddTextField@UIMenu@@QAEPAVUITextField@@HPAULocString@@PADMMMMHHHHHVCallback@@@Z
    ARTS_IMPORT UITextField* AddTextField(i32 arg1, LocString* arg2, char* arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7,
        i32 arg8, i32 arg9, i32 arg10, i32 arg11, i32 arg12, Callback arg13);

    // ?AddTextRoller@UIMenu@@QAEPAVUITextRoller@@HPAULocString@@PAHMMMMVstring@@HHHHVCallback@@@Z | unused
    ARTS_IMPORT UITextRoller* AddTextRoller(i32 arg1, LocString* arg2, i32* arg3, f32 arg4, f32 arg5, f32 arg6,
        f32 arg7, string arg8, i32 arg9, i32 arg10, i32 arg11, i32 arg12, Callback arg13);

    // ?AddTextRoller2@UIMenu@@QAEPAVUITextRoller2@@HPAULocString@@PAHMMMMVstring@@HHHHVCallback@@@Z
    ARTS_IMPORT UITextRoller2* AddTextRoller2(i32 arg1, LocString* arg2, i32* arg3, f32 arg4, f32 arg5, f32 arg6,
        f32 arg7, string arg8, i32 arg9, i32 arg10, i32 arg11, i32 arg12, Callback arg13);

    // ?AddTextScroll@UIMenu@@QAEPAVUITextScroll@@HPAXMMMMVstring@@HPAHVCallback@@@Z
    ARTS_IMPORT UITextScroll* AddTextScroll(
        i32 arg1, void* arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, string arg7, i32 arg8, i32* arg9, Callback arg10);

    // ?AddToggle@UIMenu@@QAEPAVUIToggleButton@@HPAULocString@@PAHMMMMHHVCallback@@@Z | unused
    ARTS_IMPORT UIToggleButton* AddToggle(i32 arg1, LocString* arg2, i32* arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7,
        i32 arg8, i32 arg9, Callback arg10);

    // ?AddToggle2@UIMenu@@QAEPAVUIToggleButton2@@HPAULocString@@PAHMMMMHHVCallback@@@Z
    ARTS_IMPORT UIToggleButton2* AddToggle2(i32 arg1, LocString* arg2, i32* arg3, f32 arg4, f32 arg5, f32 arg6,
        f32 arg7, i32 arg8, i32 arg9, Callback arg10);

    // ?AddUIControl@UIMenu@@QAEPAVUIControlWidget@@HMMMMMPAVmmIO@@VCallback@@@Z | unused
    ARTS_IMPORT UIControlWidget* AddUIControl(
        i32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, mmIO* arg7, Callback arg8);

    // ?AddVScrollBar@UIMenu@@QAEPAVUIVScrollBar@@HPAHMMMMMMHHVCallback@@@Z | unused
    ARTS_IMPORT UIVScrollBar* AddVScrollBar(i32 arg1, i32* arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7,
        f32 arg8, i32 arg9, i32 arg10, Callback arg11);

    // ?AddWidget@UIMenu@@QAEXPAVuiWidget@@PADMMMMH1@Z
    ARTS_EXPORT void AddWidget(
        uiWidget* widget, aconst char* label, f32 x, f32 y, f32 w, f32 h, i32 id, aconst char* icon);

    // ?AssignBackground@UIMenu@@QAEXPAD@Z
    ARTS_EXPORT void AssignBackground(aconst char* background_name);

    // ?AssignName@UIMenu@@QAEXPAULocString@@@Z
    ARTS_EXPORT void AssignName(LocString* name);

    // ?CheckMouseHits@UIMenu@@QAEXXZ
    ARTS_IMPORT void CheckMouseHits();

    // ?ClearAction@UIMenu@@QAEXXZ
    ARTS_EXPORT void ClearAction();

    // ?ClearSelected@UIMenu@@QAEXXZ
    ARTS_EXPORT void ClearSelected();

    // ?ClearToolTip@UIMenu@@QAEXXZ
    ARTS_EXPORT void ClearToolTip();

    // ?ClearWidgets@UIMenu@@QAEXXZ
    ARTS_EXPORT void ClearWidgets();

    // ?Decrement@UIMenu@@QAEHXZ
    ARTS_EXPORT b32 Decrement();

    // ?Disable@UIMenu@@QAEXXZ
    ARTS_EXPORT void Disable();

    // ?DisableIME@UIMenu@@QAEXXZ
    void DisableIME();

    // ?Enable@UIMenu@@QAEXXZ
    ARTS_EXPORT void Enable();

    // ?FindTheFirstFocusWidget@UIMenu@@QAEHXZ
    i32 FindTheFirstFocusWidget();
    i32 FindTheNextFocusWidget();
    i32 FindThePrevFocusWidget();
    i32 FindTheLastFocusWidget();
    i32 FindFocusWidget(i32 start, i32 step);

    // ?GetBstate@UIMenu@@QAEHXZ
    i32 GetBstate()
    {
        return *p_b_state_;
    }

    // ?GetDimensions@UIMenu@@QAEXAAM000@Z
    ARTS_EXPORT void GetDimensions(f32& x, f32& y, f32& w, f32& h);

    // ?GetWidgetID@UIMenu@@QAEHXZ
    ARTS_EXPORT i32 GetWidgetID()
    {
        return widget_id_;
    }

    // ?Increment@UIMenu@@QAEHXZ
    ARTS_EXPORT b32 Increment();

    // ?KeyboardAction@UIMenu@@QAEXTeqEvent@@@Z
    void KeyboardAction(eqEvent event);

    // ?MouseAction@UIMenu@@QAEXTeqEvent@@@Z
    ARTS_EXPORT void MouseAction(eqEvent event);

    // ?MouseHitCheck@UIMenu@@QAEPAVuiWidget@@HMM@Z
    ARTS_IMPORT uiWidget* MouseHitCheck(i32 arg1, f32 arg2, f32 arg3);

    // ?ScaleWidget@UIMenu@@QAEXAAM000@Z
    ARTS_EXPORT void ScaleWidget(f32& x, f32& y, f32& w, f32& h);

    // ?ScanInput@UIMenu@@QAEHPATeqEvent@@@Z
    b32 ScanInput(eqEvent* event);

    // ?SetAction@UIMenu@@QAEXW4eSource@1@@Z
    ARTS_IMPORT void SetAction(UIMenu::eSource arg1);

    // ?SetBstate@UIMenu@@QAEXH@Z
    ARTS_IMPORT void SetBstate(i32 index);

    // ?SetFocusWidget@UIMenu@@QAEXH@Z
    ARTS_IMPORT void SetFocusWidget(i32 arg1);

    // ?SetSelected@UIMenu@@QAEXXZ
    ARTS_IMPORT void SetSelected();

    // ?Update@UIMenu@@UAEXXZ
    ARTS_IMPORT void Update() override;

    void SetNavigationOrder(const char* const* labels, usize count);

    // ?UI_BOTTOM_MARGIN@UIMenu@@2MA
    ARTS_IMPORT static f32 UI_BOTTOM_MARGIN;

    // ?UI_LEFT_MARGIN@UIMenu@@2MA
    ARTS_IMPORT static f32 UI_LEFT_MARGIN;

    // ?UI_LEFT_MARGIN2@UIMenu@@2MA
    ARTS_IMPORT static f32 UI_LEFT_MARGIN2;

    // ?UI_RIGHT_MARGIN@UIMenu@@2MA
    ARTS_IMPORT static f32 UI_RIGHT_MARGIN;

    // ?UI_TOP_MARGIN@UIMenu@@2MA
    ARTS_IMPORT static f32 UI_TOP_MARGIN;

    // ?WIDGET_HEIGHT@UIMenu@@2MA
    ARTS_IMPORT static f32 WIDGET_HEIGHT;

    // ?WIDGET_WIDTH@UIMenu@@2MA
    ARTS_IMPORT static f32 WIDGET_WIDTH;

    i32 GetState() const
    {
        return state_;
    }

    i32 GetMenuID() const
    {
        return menu_id_;
    }

    i32 GetPreviousMenuID() const
    {
        return prev_menu_id_;
    }

    i32 GetWidgetCount() const
    {
        return widget_count_;
    }

    uiWidget* GetWidget(i32 index) const
    {
        return widgets_[index];
    }

    uiWidget* FindWidget(i32 id);

    uiWidget* GetActiveWidget() const
    {
        return widget_count_ ? widgets_[*p_b_state_] : nullptr;
    }

protected:
    enum
    {
        MENU_STATE_1 = 1,
        MENU_STATE_2 = 2,
        MENU_STATE_3 = 3,
    };

    i32 action_source_;
    i32 state_;
    i32 enabled_;
    i32 menu_id_;
    i32 field_30;
    i32 widget_count_;
    i32 field_38;
    Ptr<uiWidget*[]> widgets_;
    ConstString menu_name_;
    char* field_44;
    i32 field_48;
    f32 menu_x_;
    f32 menu_y_;
    f32 menu_width_;
    f32 menu_height_;
    f32 field_5C;
    f32 field_60;
    f32 field_64;
    i32 field_68;
    i32* p_b_state_;
    i32 b_state_;
    i32 field_74;
    i32 widget_id_;
    i32 prev_menu_id_;
    i32 focus_widget_index_;
    f32 field_84;
    i32 field_88;
    ConstString background_name_;
};

check_size(UIMenu, 0x90);
