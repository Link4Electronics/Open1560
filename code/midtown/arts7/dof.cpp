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

define_dummy_symbol(arts7_dof);

#include "dof.h"

#include "sim.h"

#include <cmath>

void asDofCS::FileIO(MiniParser* /*arg1*/)
{}

asDofCS::asDofCS()
{
    ClearNodeFlag(NODE_FLAG_2);

    mode = 0;
    type = 0;
    pitch_scale_ = 1.0f;
    yaw_scale_ = 0.0f;
    roll_scale_ = 0.0f;
    position_ = {};
    offset_velocity_ = {};
    current_value_ = 0.0f;
    pad_d8_ = 0.0f;
    prev_value_limit_ = 40000.0f;
    prev_position_ = {56156.4f, 30635.8f, 46984.5f};
    velocity_ = 0.0f;
    acceleration_ = 0.0f;
    oscillation_phase_ = 0.0f;
    oscillation_amplitude_ = 0.0f;
    hold_time_ = 0.0f;
    timer_ = 0.0f;
    min_bound_ = -16800.0f;
    max_bound_ = 990000.0f;
    oscillation_frequency_ = 1.0f;
    clamp_flag_ = 1;
    next_type_ = 2;
}

void asDofCS::Reset()
{
    if (mode == 2)
        current_value_ = 1.0f;
    else
        current_value_ = 0.0f;

    prev_value_limit_ = -4.0f;
    asNode::Reset();
}

MetaClass* asDofCS::GetClass()
{
    return nullptr;
}

void asDofCS::DeclareFields()
{}

#ifdef ARTS_DEV_BUILD
void asDofCS::AddWidgets(Bank* /*arg1*/)
{}

void asDofCS::ModeCB()
{}
#endif

void asDofCS::Update()
{
    f32 dt = static_cast<f32>(Sim()->GetUpdateDelta());

    // State machine: handle timer and velocity
    if (timer_ > 0.0f)
    {
        timer_ -= dt;

        if (timer_ <= 0.0f)
        {
            timer_ = hold_time_;
            type = next_type_;
        }
    }
    else if (velocity_ != 0.0f)
    {
        current_value_ += velocity_ * dt;

        if (clamp_flag_)
        {
            if (current_value_ < min_bound_)
            {
                current_value_ = min_bound_;
                velocity_ = -velocity_;
            }
            else if (current_value_ > max_bound_)
            {
                current_value_ = max_bound_;
                velocity_ = -velocity_;
            }
        }
    }

    // Oscillation
    if (oscillation_amplitude_ > 0.0f)
    {
        oscillation_phase_ += oscillation_frequency_ * dt;
        current_value_ += oscillation_amplitude_ * std::sin(oscillation_phase_);
    }

    prev_value_limit_ = current_value_;

    // Apply animation to local matrix
    // (RotateAbs/ScaleFullAbs are weak no-ops on standalone, pending full implementation)
    Matrix.RotateAbs(Vector3{pitch_scale_, yaw_scale_, roll_scale_}, current_value_);
    Matrix.m3 = {};
    Matrix.m3.x = position_.x;
    Matrix.m3.y = position_.y;
    Matrix.m3.z = position_.z;

    prev_position_ = position_;

    asLinearCS::Update();
}
