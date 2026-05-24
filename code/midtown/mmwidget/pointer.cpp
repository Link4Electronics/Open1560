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

define_dummy_symbol(mmwidget_pointer);

#include "pointer.h"

#include "agi/bitmap.h"
#include "agi/pipeline.h"
#include "arts7/cullmgr.h"
#include "eventq7/event.h"

#include <fcntl.h>
#include <unistd.h>

sfPointer::sfPointer()
{
    ActivateNode();
}

sfPointer::~sfPointer() = default;

void sfPointer::Init()
{
    CursorTexture = as_rc Pipe()->GetBitmap("midcursor", 0.05f, 0.0666667f, 1);
    if (!CursorTexture)
        Warningf("sfPointer: could not load midcursor");
    MaxX = Pipe()->GetWidth();
    MaxY = Pipe()->GetHeight();
    State = 0;
    CurrentPos = {0.0f, 0.0f};
    PrevPos = {0.0f, 0.0f};
    CurrentWidget = nullptr;
}

void sfPointer::Update()
{
    if (!IsNodeActive())
        return;

    PrevPos = CurrentPos;

    if (eqEventHandler::SuperQ)
    {
        f32 mouse_x = eqEventHandler::SuperQ->GetMouseX();
        f32 mouse_y = eqEventHandler::SuperQ->GetMouseY();

        CurrentPos.x = std::clamp(mouse_x * static_cast<f32>(Pipe()->GetWidth()) / 640.0f, 0.0f, static_cast<f32>(Pipe()->GetWidth() - 1));
        CurrentPos.y = std::clamp(mouse_y * static_cast<f32>(Pipe()->GetHeight()) / 480.0f, 0.0f, static_cast<f32>(Pipe()->GetHeight() - 1));
    }

    if (CursorTexture)
        CullMgr()->DeclareBitmap(this, CursorTexture.get());
    CullMgr()->DeclareCullable2D(this);
    asNode::Update();
}

void sfPointer::Cull()
{
    if (!IsNodeActive())
        return;

    if (State == 2)
        return;

    if (CursorTexture)
    {
        i32 cx = static_cast<i32>(CurrentPos.x);
        i32 cy = static_cast<i32>(CurrentPos.y);
        Pipe()->CopyClippedBitmap(cx, cy, CursorTexture.get(), 0, 0,
            CursorTexture->GetWidth(), CursorTexture->GetHeight());
    }
}

f32 sfPointer::GetPointerHeight()
{
    return 0.0f;
}

void sfPointer::ResChange(i32, i32) {} // ARTS_IMPORT stub