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

#include "linear.h"

class asDofCS final : public asLinearCS
{
public:
    // ??0asDofCS@@QAE@XZ
    asDofCS();

    // ??1asDofCS@@UAE@XZ | inline
    ARTS_EXPORT ~asDofCS() override = default;

#ifdef ARTS_DEV_BUILD
    // ?AddWidgets@asDofCS@@UAEXPAVBank@@@Z
    ARTS_IMPORT void AddWidgets(Bank* arg1) override;
#endif

    // ?FileIO@asDofCS@@QAEXPAVMiniParser@@@Z | unused
    void FileIO(MiniParser* arg1);

    // ?GetClass@asDofCS@@UAEPAVMetaClass@@XZ
    ARTS_IMPORT MetaClass* GetClass() override;

    // ?Reset@asDofCS@@UAEXXZ
    void Reset() override;

    // ?Update@asDofCS@@UAEXXZ
    void Update() override;

    // ?DeclareFields@asDofCS@@SAXXZ
    ARTS_IMPORT static void DeclareFields();

protected:
#ifdef ARTS_DEV_BUILD
    // ?ModeCB@asDofCS@@IAEXXZ
    ARTS_IMPORT void ModeCB();
#endif

    // Fields at offset 0x88 (after asLinearCS's 0x88 bytes)
    i32 mode {};                    // 0x88 - 0x8B: behavior mode (0=position, 1=rotation, 2=camera-look)
    i32 type {};                    // 0x8C - 0x8F: motion type (0=rotate, 1=oneshot, 2=hold, 3=bounce)
    f32 pitch_scale_ {1.0f};       // 0x90 - 0x93: scale factor for pitch axis
    f32 yaw_scale_ {};             // 0x94 - 0x97: scale factor for yaw axis
    f32 roll_scale_ {};            // 0x98 - 0x9B: scale factor for roll axis
    Vector3 position_ {};          // 0x9C - 0xA7: target position offset
    Vector3 offset_velocity_ {};   // 0xA8 - 0xB3: per-axis velocity offset
    Vector3 prev_position_ {56156.4f, 30635.8f, 46984.5f}; // 0xB4 - 0xBF: previous frame position
    f32 current_value_ {};         // 0xC0 - 0xC3: current animation value (angle/position)
    f32 prev_value_limit_ {40000.0f}; // 0xC4 - 0xC7: previous value clamp limit
    f32 min_bound_ {-16800.0f};    // 0xC8 - 0xCB: minimum bound
    f32 max_bound_ {990000.0f};    // 0xCC - 0xCF: maximum bound
    f32 velocity_ {};              // 0xD0 - 0xD3: current velocity
    f32 acceleration_ {};          // 0xD4 - 0xD7: target acceleration
    f32 pad_d8_ {};                // 0xD8 - 0xDB: unused padding
    f32 oscillation_amplitude_ {}; // 0xDC - 0xDF: amplitude of oscillation
    f32 oscillation_frequency_ {1.0f}; // 0xE0 - 0xE3: frequency of oscillation
    f32 oscillation_phase_ {};     // 0xE4 - 0xE7: phase of oscillation
    f32 timer_ {};                 // 0xE8 - 0xEB: elapsed time
    f32 hold_time_ {};             // 0xEC - 0xEF: hold duration after animation
    i32 clamp_flag_ {1};           // 0xF0 - 0xF3: clamp to bounds flag
    i32 next_type_ {2};            // 0xF4 - 0xF7: type to transition to after hold
};

check_size(asDofCS, 0xF8);
