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

define_dummy_symbol(agi_lmodel);

#include "lmodel.h"

agiLightModelParameters::agiLightModelParameters() = default;

void agiLightModelParameters::operator=(const agiLightModelParameters& arg1)
{
    Ambient = arg1.Ambient;
    LocalViewer = arg1.LocalViewer;
    dword14 = arg1.dword14;
    dword18 = arg1.dword18;
    Enabled = arg1.Enabled;
    Monochromatic = arg1.Monochromatic;
    Changed = arg1.Changed;
}

agiLightModel::~agiLightModel() = default;

aconst char* agiLightModel::GetName()
{
    static char buffer[64];
    arts_sprintf(buffer, "LightModel '%p'", this);
    return buffer;
}

i32 agiLightModel::Init(const agiLightModelParameters& arg1)
{
    EndGfx();
    Params = arg1;
    return SafeBeginGfx();
}

agiLightModel::agiLightModel(agiPipeline* arg1)
    : agiRefreshable(arg1)
{}
