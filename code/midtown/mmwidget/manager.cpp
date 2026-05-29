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

define_dummy_symbol(mmwidget_manager);

#include "manager.h"

#include "agi/pipeline.h"
#include "arts7/camera.h"
#include "arts7/cullmgr.h"
#include "arts7/lamp.h"
#include "arts7/linear.h"
#include "arts7/view.h"
#include "eventq7/eventq.h"
#include "mmeffects/card2d.h"
#include "mmeffects/mmtext.h"
#include "mmaudio/sound.h"

#include <unistd.h>
#include <fcntl.h>

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>

#include "stream/fsystem.h"
#include "stream/stream.h"

#include "bm_button.h"
#include "menu.h"
#include "mstore.h"
#include "navbar.h"
#include "pointer.h"
#include "widget.h"
#include "wstore.h"

#include "vector7/vector4.h"

Vector4& MenuManager::GetFGColor(i32) { static Vector4 white{1,1,1,1}; return white; } // ARTS_IMPORT stub

static SDL_AudioStream* g_UIOptionsStream = nullptr;
static u8* g_UIOptionsData = nullptr;
static int g_UIOptionsLen = 0;

static SDL_AudioStream* g_UIDriverStream = nullptr;
static u8* g_UIDriverData = nullptr;
static int g_UIDriverLen = 0;

static void PlayOptionsSoundNow()
{
    if (!g_UIOptionsStream)
    {
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
            return;

        const char* paths[] = {
            "AUD/AUD22/UIOPTIONS.22K.WAV",
            "AUD/AUD11/UIOPTIONS.11K.WAV",
        };

        for (const char* vfs_path : paths)
        {
            Ptr<Stream> stream = as_ptr FileSystem::OpenAny(vfs_path, true, nullptr, 4096);
            if (!stream)
                continue;

            i32 file_size = stream->Size();
            if (file_size <= 0)
                continue;

            Ptr<u8[]> file_data = arnewa u8[file_size];
            stream->Read(file_data.get(), file_size);

            if (file_size <= 12 || file_data[0] != 'R' || file_data[1] != 'I' || file_data[2] != 'F' || file_data[3] != 'F')
                continue;

            u16 num_channels = 1;
            u32 sample_rate = 22050;
            u16 bits_per_sample = 16;
            i32 data_offset = 0;
            i32 data_size = 0;
            i32 offset = 12;

            while (offset + 8 <= file_size)
            {
                u32 chunk_id = *reinterpret_cast<u32*>(&file_data[offset]);
                u32 chunk_size = *reinterpret_cast<u32*>(&file_data[offset + 4]);

                if (chunk_id == 0x20746D66 && offset + 24 <= file_size)
                {
                    u16 audio_format = *reinterpret_cast<u16*>(&file_data[offset + 8]);
                    if (audio_format != 1)
                        break;
                    num_channels = *reinterpret_cast<u16*>(&file_data[offset + 10]);
                    sample_rate = *reinterpret_cast<u32*>(&file_data[offset + 12]);
                    bits_per_sample = *reinterpret_cast<u16*>(&file_data[offset + 22]);
                }
                else if (chunk_id == 0x61746164 && chunk_size > 0)
                {
                    data_offset = offset + 8;
                    data_size = chunk_size;
                    break;
                }

                offset += 8 + chunk_size;
                if (chunk_size % 2)
                    ++offset;
            }

            if (data_size <= 0)
                continue;

            SDL_AudioSpec spec;
            SDL_zero(spec);
            spec.format = (bits_per_sample == 16) ? SDL_AUDIO_S16 : SDL_AUDIO_U8;
            spec.channels = num_channels;
            spec.freq = static_cast<int>(sample_rate);

            g_UIOptionsStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
            if (!g_UIOptionsStream)
                continue;

            g_UIOptionsLen = data_size;
            g_UIOptionsData = static_cast<u8*>(SDL_malloc(data_size));
            if (g_UIOptionsData)
                SDL_memcpy(g_UIOptionsData, &file_data[data_offset], data_size);

            SDL_ResumeAudioStreamDevice(g_UIOptionsStream);
            break;
        }
    }

    if (g_UIOptionsStream && g_UIOptionsData)
    {
        int queued = SDL_GetAudioStreamQueued(g_UIOptionsStream);
        if (queued < g_UIOptionsLen)
            SDL_PutAudioStreamData(g_UIOptionsStream, g_UIOptionsData, g_UIOptionsLen);
    }
}

static void PlayDriverSoundNow()
{
    if (!g_UIDriverStream)
    {
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
            return;

        const char* paths[] = {
            "AUD/AUD22/UIDRIVER.22K.WAV",
            "AUD/AUD11/UIDRIVER.11K.WAV",
        };

        for (const char* vfs_path : paths)
        {
            Ptr<Stream> stream = as_ptr FileSystem::OpenAny(vfs_path, true, nullptr, 4096);
            if (!stream)
                continue;

            i32 file_size = stream->Size();
            if (file_size <= 0)
                continue;

            Ptr<u8[]> file_data = arnewa u8[file_size];
            stream->Read(file_data.get(), file_size);

            if (file_size <= 12 || file_data[0] != 'R' || file_data[1] != 'I' || file_data[2] != 'F' || file_data[3] != 'F')
                continue;

            u16 num_channels = 1;
            u32 sample_rate = 22050;
            u16 bits_per_sample = 16;
            i32 data_offset = 0;
            i32 data_size = 0;
            i32 offset = 12;

            while (offset + 8 <= file_size)
            {
                u32 chunk_id = *reinterpret_cast<u32*>(&file_data[offset]);
                u32 chunk_size = *reinterpret_cast<u32*>(&file_data[offset + 4]);

                if (chunk_id == 0x20746D66 && offset + 24 <= file_size)
                {
                    u16 audio_format = *reinterpret_cast<u16*>(&file_data[offset + 8]);
                    if (audio_format != 1)
                        break;
                    num_channels = *reinterpret_cast<u16*>(&file_data[offset + 10]);
                    sample_rate = *reinterpret_cast<u32*>(&file_data[offset + 12]);
                    bits_per_sample = *reinterpret_cast<u16*>(&file_data[offset + 22]);
                }
                else if (chunk_id == 0x61746164 && chunk_size > 0)
                {
                    data_offset = offset + 8;
                    data_size = chunk_size;
                    break;
                }

                offset += 8 + chunk_size;
                if (chunk_size % 2)
                    ++offset;
            }

            if (data_size <= 0)
                continue;

            SDL_AudioSpec spec;
            SDL_zero(spec);
            spec.format = (bits_per_sample == 16) ? SDL_AUDIO_S16 : SDL_AUDIO_U8;
            spec.channels = num_channels;
            spec.freq = static_cast<int>(sample_rate);

            g_UIDriverStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
            if (!g_UIDriverStream)
                continue;

            g_UIDriverLen = data_size;
            g_UIDriverData = static_cast<u8*>(SDL_malloc(data_size));
            if (g_UIDriverData)
                SDL_memcpy(g_UIDriverData, &file_data[data_offset], data_size);

            SDL_ResumeAudioStreamDevice(g_UIDriverStream);
            break;
        }
    }

    if (g_UIDriverStream && g_UIDriverData)
    {
        int queued = SDL_GetAudioStreamQueued(g_UIDriverStream);
        if (queued < g_UIDriverLen)
            SDL_PutAudioStreamData(g_UIDriverStream, g_UIDriverData, g_UIDriverLen);
    }
}

void MenuManager::PlayMenuSwitchSound()
{
    if (active_menu_id_ == IDM_OPTIONS)
        PlayOptionsSoundNow();
    else if (active_menu_id_ == IDM_DRIVER)
        PlayDriverSoundNow();
}

void* MenuManager::GetFont(i32 size)
{
    void** font_ptr = nullptr;

    switch (size)
    {
        case 12: font_ptr = &font_size_12_; break;
        case 14: font_ptr = &font_size_14_; break;
        case 16: font_ptr = &font_size_16_; break;
        case 20: font_ptr = &font_size_20_; break;
        case 24: font_ptr = &font_size_24_; break;
        case 32: font_ptr = &font_size_32_; break;
        case 48: font_ptr = &font_size_48_; break;
        case 64: font_ptr = &font_size_64_; break;
    }

    if (font_ptr)
    {
        if (!*font_ptr)
            *font_ptr = mmText::CreateFont("Gill Sans MT", size);

        return *font_ptr;
    }

    return nullptr;
}

MenuManager::~MenuManager()
{
}

MenuManager::MenuManager()
{
    void* saved_vtable = *reinterpret_cast<void**>(this);
    std::memset(this, 0, sizeof(*this));
    *reinterpret_cast<void**>(this) = saved_vtable;

    ActivateNode();

    Instance = this;

    event_q_ = arnew eqEventQ(1, ~0u, 64);
    menu_camera_ = arnew asCamera();

    popup_ = arnew Card2D();
    popup_->SetNodeFlag(NODE_FLAG_UPDATE_PAUSED);

    active_menu_id_ = -1;
    next_active_menu_id_ = -1;

    nav_bar_ = arnew uiNavBar();
    AddChild(nav_bar_.get());
}

void MenuManager::CheckInput()
{
    if (dialog_menu_)
    {
        dialog_menu_->CheckInput();
        dialog_menu_->CheckMouseHits();
    }
    else if (UIMenu* menu = GetCurrentMenu())
    {
        menu->CheckInput();
        menu->CheckMouseHits();
    }

    if (nav_bar_)
        nav_bar_->CheckMouseHits();
}

void MenuManager::CheckBG(UIMenu* menu)
{
    if (menu && menu->GetBackgroundName() && menu->GetBackgroundName()[0])
        SetBackgroundImage(const_cast<char*>(menu->GetBackgroundName()));
}

void MenuManager::SetDefaultBackgroundImage(char* path)
{
    default_background_ = path;
    SetBackgroundImage(path);
}

void MenuManager::SetBackgroundImage(char* path)
{
    if ((!path || !*path) && default_background_)
        path = default_background_;

    if (path && *path && menu_camera_)
        menu_camera_->SetUnderlay(path);
}

void MenuManager::ResChange(i32 /*width*/, i32 /*height*/)
{
}

i32 MenuManager::AddMenu2(UIMenu* menu)
{
    if (!menus_)
    {
        if (max_menus_ <= 0)
            max_menus_ = 16;
        menus_ = arnewa UIMenu*[max_menus_] {};
    }
    else if (num_menus_ >= max_menus_)
    {
        i32 new_max = max_menus_ * 2;
        Ptr<UIMenu*[]> new_menus = arnewa UIMenu*[new_max] {};
        for (i32 i = 0; i < num_menus_; ++i)
            new_menus[i] = menus_[i];
        menus_ = std::move(new_menus);
        max_menus_ = new_max;
    }

    i32 index = num_menus_++;
    menus_[index] = menu;
    return index;
}

i32 MenuManager::FindMenu(i32 idm)
{
    for (i32 i = 0; i < num_menus_; ++i)
    {
        if (menus_[i] && menus_[i]->GetMenuID() == idm)
            return i;
    }
    return -1;
}

void MenuManager::AddPointer()
{
    if (pointer_)
        return;

    pointer_ = arnew sfPointer();
    pointer_->Init();
    AddChild(pointer_.get());
}

void MenuManager::AdjustPopupCard(UIMenu* menu)
{
    f32 x, y, w, h;
    menu->GetDimensions(x, y, w, h);
    CheckBG(menu);
    popup_->SetDimensions(x, y, w, h);
}

#ifdef ARTS_DEV_BUILD
void MenuManager::AddWidgets(Bank* /*arg1*/)
{}
#endif

// ?HelpWatcher@@YGKPAX@Z
ARTS_IMPORT /*static*/ ulong ARTS_STDCALL HelpWatcher(void* arg1);

i32 MenuManager::GetPreviousMenu()
{
    if (UIMenu* menu = GetCurrentMenu())
        return menu->GetPreviousMenuID();

    return -1;
}

void MenuManager::GetScale(f32& x, f32& y, f32& width, f32& height)
{
    if (Is3D())
    {
        x = start_x_;
        y = start_y_;

        width = scale_x_;
        height = scale_y_;
    }
    else
    {
        x = 0.0f;
        y = 0.0f;

        width = 1.0f;
        height = 1.0f;
    }

    // TODO: Move scaling to MenuManager::Init?
    x = UI_StartX + (x * UI_ScaleX);
    y = UI_StartY + (y * UI_ScaleY);

    width = width * UI_ScaleX;
    height = height * UI_ScaleY;
}

void MenuManager::SetFocus(UIMenu* menu)
{
    active_menu_ = menu;
}

i32 MenuManager::Switch(i32 id)
{
    // The logic for updating popups is wierd/slightly broken.
    // If PUControls is opened and closed using F1, it locks up the menu system because the MenuManager is no longer being updated
    if (IsPopupOpen() || active_menu_id_ < 0)
    {
        SwitchNow(id);
    }
    else
    {
        next_active_menu_id_ = id;
    }

    return id;
}

void MenuManager::ToggleFocus(i32 direction)
{
    UIMenu* focus = nav_bar_.get();

    if (active_menu_ == focus || !focus || !focus->IsNodeActive())
        focus = GetCurrentMenu();

    if (focus)
        SetFocus(focus);

    focus = active_menu_;

    if (direction == 0)
        focus->FindTheFirstFocusWidget();
    else if (direction == -1)
        focus->FindTheLastFocusWidget();
}

void MenuManager::Update()
{
    ForceCurrentFocus();

    last_drawn_ = nullptr;

    asNode::Update();

    if (menu_camera_)
        CullMgr()->DeclareCamera(menu_camera_.get());

    for (i32 i = 0; i < num_menus_; ++i)
    {
        if (menus_[i] && menus_[i]->IsNodeActive())
            menus_[i]->Update();
    }

    if (last_drawn_)
        last_drawn_->Update();

    if (widget_snap_)
    {
        if (UIMenu* menu = active_menu_)
        {
            if (uiWidget* focus = menu->GetActiveWidget())
            {
                auto sq = eqEventHandler::SuperQ;
                f32 x = (focus->MinX + focus->MaxX) * sq->GetCenterX();
                f32 y = (focus->MinY + focus->MaxY) * sq->GetCenterY();
                sq->WarpMouse(x, y, false);
            }
        }
    }

    if (pointer_)
        pointer_->Update();
}

void MenuManager::SwitchNow(i32 id)
{
    if (id == active_menu_id_)
        return;

    if (!GetMenu(id))
        return;

    bool had_previous = false;

    if (num_menus_ > 0 && active_menu_id_ >= 0)
    {
        had_previous = true;
        Disable(active_menu_id_);
    }

    if (UIMenu* new_menu = GetMenu(id))
    {
        // Only initialize prev_menu_id_ on first entry — preserve any existing value
        // so re-entering a menu doesn't corrupt the navigation chain.
        if (new_menu->GetPreviousMenuID() < 0)
            new_menu->SetPreviousMenuID(active_menu_id_);
    }

    Enable(id);
    active_menu_id_ = id;

    if (had_previous && !IsPopupOpen())
        PlayMenuSwitchSound();

    if (had_previous || IsPopupOpen())
    {
        if (active_menu_)
        {
            active_menu_->ClearAction();
            active_menu_->ClearWidgets();
        }

        SetFocus(GetCurrentMenu());
        active_menu_->SetSelected();
    }
}

void MenuManager::ForceCurrentFocus()
{
    if (next_active_menu_id_ != -1)
    {
        SwitchNow(next_active_menu_id_);
        next_active_menu_id_ = -1;
    }
}

UIMenu* MenuManager::GetCurrentMenu()
{
    i32 index = FindMenu(active_menu_id_);

    return (index >= 0) ? menus_[index] : nullptr;
}

void MenuManager::Enable(i32 id)
{
    i32 index = FindMenu(id);

    if (index < 0)
        return;

    active_menu_id_ = id;

    if (!Is3D() || IsPopupOpen())
    {
        menus_[index]->Enable();

        if (Is3D())
            AdjustPopupCard(menus_[index]);

        CheckBG(menus_[index]);
    }
}

i32 MenuManager::CurrentMenuSelected()
{
    if (dialog_menu_)
        return dialog_menu_->GetMenuID();

    return active_menu_id_;
}

void MenuManager::Disable(i32 id)
{
    if (id == -1)
        id = active_menu_id_;

    if (i32 index = FindMenu(id); index >= 0)
        menus_[index]->Disable();
}

void MenuManager::Kill()
{
    event_q_ = nullptr;
    menu_cs_ = nullptr;
    lamps_ = nullptr;
    lcss_ = nullptr;

    move_selector_sound_ = nullptr;
    selection_made_sound_ = nullptr;
    switch_sound_ = nullptr;
    menu_camera_ = nullptr;
}

i32 MenuManager::MenuState(i32 menu)
{
    if (dialog_menu_)
        return dialog_menu_->GetState();

    if (menu == -1)
        menu = active_menu_id_;

    if (i32 index = FindMenu(menu); index >= 0)
        return menus_[index]->GetState();

    return 0;
}

uiWidget* MenuManager::MouseAction(i32 button, f32 x, f32 y)
{
    if (dialog_menu_)
        return dialog_menu_->MouseHitCheck(button, x, y);

    if (i32 index = FindMenu(active_menu_id_); index >= 0)
    {
        if (uiWidget* widget = menus_[index]->MouseHitCheck(button, x, y))
            return widget;
    }

    if (nav_bar_)
    {
        // Coordinates come from the current menu's CheckInput, transformed by that menu's dimensions.
        // The nav bar uses absolute coordinates (menu_x_=0, menu_y_=0, menu_width_=1, menu_height_=1),
        // so reverse-transform to absolute space using the current menu's dimensions.
        if (UIMenu* menu = GetCurrentMenu())
        {
            f32 mx, my, mw, mh;
            menu->GetDimensions(mx, my, mw, mh);
            f32 abs_x = x * mw + mx;
            f32 abs_y = y * mh + my;
            return nav_bar_->MouseHitCheck(button, abs_x, abs_y);
        }

        return nav_bar_->MouseHitCheck(button, x, y);
    }

    return nullptr;
}

void MenuManager::RegisterWidgetFocus(b32 focused, f32 x, f32 y, f32 w, f32 h, uiWidget* widget)
{
    if (!focused || w == 0.0)
    {
        focused_widget_ = nullptr;
        has_focused_widget_ = false;
    }
    else
    {
        uiWidget* active = active_widget_;
        has_focused_widget_ = true;
        focused_widget_ = widget;

        active->MinX = x;
        active->MinY = y;
        active->MaxX = x + w;
        active->MaxY = y + h;
    }
}
