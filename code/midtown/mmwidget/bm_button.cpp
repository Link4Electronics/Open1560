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

define_dummy_symbol(mmwidget_bm_button);

#include "bm_button.h"

#include "agi/bitmap.h"
#include "agi/pipeline.h"
#include "arts7/cullmgr.h"
#include "eventq7/eventq.h"
#include "menu.h"

#include "stream/fsystem.h"
#include "stream/stream.h"

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>

#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

UIBMButton::UIBMButton()
{
    Enabled = 1;
    Active = false;
    ReadOnly = 0;
    MouseHit = 0;
}

UIBMButton::~UIBMButton()
{
    Kill();
}

void UIBMButton::Init(char* name, f32 x, f32 y, i32 type, i32 /*arg5*/, i32* /*arg6*/, i32 /*arg7*/, i32 /*arg8*/,
    LocString* /*arg9*/, Callback /*cb_1*/, Callback /*cb_2*/)
{
    Label = name;
    X = x;
    Y = y;
    MinX = x;
    MinY = y;
    MaxX = x + 0.12f;
    MaxY = y + 0.05f;
    Width = MaxX - MinX;
    Height = MaxY - MinY;

    div_type_ = type;

    // Load button bitmap
    if (name && *name)
    {
        bitmap_ = as_rc Pipe()->GetBitmap(name, 0.0f, 0.0f, 0);
        if (bitmap_)
        {
            bitmap_width_ = bitmap_->GetWidth();
            bitmap_height_ = bitmap_->GetHeight();
            i32 div = GetDiv();
            frame_height_ = (div > 0) ? (bitmap_height_ / div) : bitmap_height_;

            if (frame_height_ > 0 && bitmap_width_ > 0)
            {
                Width = static_cast<f32>(bitmap_width_) / 640.0f;
                Height = Width * frame_height_ / bitmap_width_;
                MaxX = MinX + Width;
                MaxY = MinY + Height;
            }

        }
    }
}

void UIBMButton::Update()
{
    CullMgr()->DeclareCullable2D(this);
}

void UIBMButton::Cull()
{
    if (!Enabled)
        return;

    f32 sx = MinX, sy = MinY, sw = Width, sh = Height;
    if (Menu)
        Menu->ScaleWidget(sx, sy, sw, sh);

    i32 px = static_cast<i32>(sx * Pipe()->GetWidth());
    i32 py = static_cast<i32>(sy * Pipe()->GetHeight());
    i32 pw = static_cast<i32>(sw * Pipe()->GetWidth());
    i32 ph = static_cast<i32>(sh * Pipe()->GetHeight());

    if (bitmap_)
    {
        i32 num_frames = (div_type_ > 0) ? div_type_ : 1;
        i32 frame_h = (num_frames > 1) ? (bitmap_height_ / num_frames) : bitmap_height_;
        i32 frame = 0;
        if (num_frames > 1)
        {
            frame = num_frames - 1 - state_;
            if (frame < 0) frame = 0;
            if (frame >= num_frames) frame = num_frames - 1;
        }

        if (div_type_ < 2 && bitmap_height_ > 50)
            Pipe()->ClearRect(px, py, pw, ph, 0x0000FF00);
        else
            Pipe()->ClearRect(px, py, pw, ph, 0x000000FF);

        i32 src_y = frame * frame_h;

        Pipe()->StretchCopyBitmap(px, py, pw, ph, bitmap_.get(), 0, src_y, bitmap_width_, frame_h);
    }
    else
    {
        u32 color = Active ? 0x004488CC : 0x00333333;
        Pipe()->ClearRect(px, py, pw, ph, color);
    }
}

void UIBMButton::Action(eqEvent arg1)
{
    if (arg1.Type == eqEventType::Keyboard && arg1.Key.Key == EQ_VK_RETURN)
    {
        if (Menu)
            Menu->SetAction(UIMenu::eSource::Keyboard);
    }
}

void UIBMButton::Switch(b32 active)
{
    if (active != Active)
    {
        Active = active;
        if (active)
        {
            state_ = 1;
            if (MouseHit)
                PlaySound();
        }
        else
        {
            state_ = 0;
            if (!MouseHit)
                anim_counter_ = 0;
        }
    }
}

void UIBMButton::Enable()
{
    Enabled = 1;
}

void UIBMButton::Disable()
{
    Enabled = 0;
}

void UIBMButton::SetPosition(f32 arg1, f32 arg2)
{
    f32 dx = arg1 - X;
    f32 dy = arg2 - Y;
    X = arg1;
    Y = arg2;
    MinX += dx;
    MinY += dy;
    MaxX += dx;
    MaxY += dy;
}

char* UIBMButton::ReturnDescription()
{
    return const_cast<char*>(Label);
}

f32 UIBMButton::GetScreenHeight()
{
    return MaxY - MinY;
}

MetaClass* UIBMButton::GetClass()
{
    return nullptr;
}

void UIBMButton::LoadBitmap(char* /*arg1*/)
{
}

void UIBMButton::GetSize()
{
    if (bitmap_)
    {
        bitmap_width_ = bitmap_->GetWidth();
        bitmap_height_ = bitmap_->GetHeight();
        i32 div = GetDiv();
        frame_height_ = (div > 0) ? (bitmap_height_ / div) : bitmap_height_;
    }
}

static SDL_AudioStream* g_UIBeepStream = nullptr;
static u8* g_BeepData = nullptr;
static int g_BeepLen = 0;

static SDL_AudioStream* g_ClickBeepStream = nullptr;
static u8* g_ClickBeepData = nullptr;
static int g_ClickBeepLen = 0;

static bool LoadSoundFile(const char* vfs_path, SDL_AudioStream*& out_stream, u8*& out_data, int& out_len)
{
    Ptr<Stream> stream = as_ptr FileSystem::OpenAny(vfs_path, true, nullptr, 4096);
    if (!stream)
        return false;

    i32 file_size = stream->Size();
    if (file_size <= 0)
        return false;

    Ptr<u8[]> file_data = arnewa u8[file_size];
    stream->Read(file_data.get(), file_size);

    if (file_size <= 12 || file_data[0] != 'R' || file_data[1] != 'I' || file_data[2] != 'F' || file_data[3] != 'F')
        return false;

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
        return false;

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.format = (bits_per_sample == 16) ? SDL_AUDIO_S16 : SDL_AUDIO_U8;
    spec.channels = num_channels;
    spec.freq = static_cast<int>(sample_rate);

    out_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    if (!out_stream)
        return false;

    out_len = data_size;
    out_data = static_cast<u8*>(SDL_malloc(data_size));
    if (out_data)
        SDL_memcpy(out_data, &file_data[data_offset], data_size);

    SDL_ResumeAudioStreamDevice(out_stream);
    return true;
}

void UIBMButton::PlaySound()
{
    if (!g_UIBeepStream)
        AllocateSounds();

    if (g_UIBeepStream && g_BeepData)
    {
        int queued = SDL_GetAudioStreamQueued(g_UIBeepStream);
        if (queued < g_BeepLen * 2)
            SDL_PutAudioStreamData(g_UIBeepStream, g_BeepData, g_BeepLen);
    }
}

void UIBMButton::PlayClickSound()
{
    if (!g_ClickBeepStream)
        AllocateSounds();

    if (g_ClickBeepStream && g_ClickBeepData)
    {
        int queued = SDL_GetAudioStreamQueued(g_ClickBeepStream);
        if (queued < g_ClickBeepLen * 2)
            SDL_PutAudioStreamData(g_ClickBeepStream, g_ClickBeepData, g_ClickBeepLen);
    }
}

void UIBMButton::AllocateSounds()
{
    if (g_UIBeepStream && g_ClickBeepStream)
        return;

    if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
        return;

    // Load hover sound (MOVESELECTOR)
    {
        const char* paths[] = {
            "AUD/AUD22/MOVESELECTOR.22K.WAV",
            "AUD/AUD11/MOVESELECTOR.11K.WAV",
        };

        bool loaded = false;
        for (const char* vfs_path : paths)
        {
            if (LoadSoundFile(vfs_path, g_UIBeepStream, g_BeepData, g_BeepLen))
            {
                loaded = true;
                break;
            }
        }

        if (!loaded)
        {
            // Fallback: generate a short 440Hz sine wave beep (~80ms)
            SDL_AudioSpec spec;
            SDL_zero(spec);
            spec.format = SDL_AUDIO_S16;
            spec.channels = 2;
            spec.freq = 22050;

            g_UIBeepStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
            if (g_UIBeepStream)
            {
                int sample_count = static_cast<int>(spec.freq * 80 / 1000);
                g_BeepLen = sample_count * 2 * 2;
                g_BeepData = static_cast<u8*>(SDL_malloc(g_BeepLen));

                if (g_BeepData)
                {
                    i16* samples = reinterpret_cast<i16*>(g_BeepData);
                    for (int i = 0; i < sample_count; ++i)
                    {
                        f32 t = static_cast<f32>(i) / static_cast<f32>(spec.freq);
                        f32 envelope = 1.0f - (static_cast<f32>(i) / static_cast<f32>(sample_count));
                        i16 sample = static_cast<i16>(std::sin(t * 440.0f * 2.0f * ARTS_PI) * 16000.0f * envelope);
                        samples[i * 2 + 0] = sample;
                        samples[i * 2 + 1] = sample;
                    }

                    SDL_ResumeAudioStreamDevice(g_UIBeepStream);
                }
            }
        }
    }

    // Load click sound (SELECTIONMADE)
    if (!g_ClickBeepStream)
    {
        const char* paths[] = {
            "AUD/AUD22/SELECTIONMADE.22K.WAV",
            "AUD/AUD11/SELECTIONMADE.11K.WAV",
        };

        for (const char* vfs_path : paths)
        {
            if (LoadSoundFile(vfs_path, g_ClickBeepStream, g_ClickBeepData, g_ClickBeepLen))
                break;
        }
    }
}

AudSound* UIBMButton::s_pSound {nullptr};

void UIBMButton::DeclareFields()
{
}

void UIBMButton::GetHitArea(f32& arg1, f32& arg2)
{
    arg1 = 0.0f;
    arg2 = 0.0f;
}

i32 UIBMButton::GetDiv()
{
    return div_type_;
}

void UIBMButton::DoToggle()
{
}

void UIBMButton::MexOff()
{
}

void UIBMButton::MexOn()
{
}

void UIBMButton::Unkill()
{
}

void UIBMButton::Kill()
{
    bitmap_ = nullptr;
    state_ = 4;
}

agiBitmap* UIBMButton::CreateDummyBitmap()
{
    return nullptr;
}
