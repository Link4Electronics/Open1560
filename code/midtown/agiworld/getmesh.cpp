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

define_dummy_symbol(agiworld_getmesh);

#include "getmesh.h"

#include "agiworld/meshset.h"
#include "agiworld/texsheet.h"
#include "stream/fsystem.h"
#include "stream/stream.h"

// ?MeshCurrentObject@@3PADA
char* MeshCurrentObject {};

// ?MeshBytesPaged@@3HA
i32 MeshBytesPaged {};

// ?MeshesPaged@@3HA
i32 MeshesPaged {};

// ?GetMeshSet@@YAPAVagiMeshSet@@PAD0PAVVector3@@H@Z
agiMeshSet* GetMeshSet(aconst char* name, aconst char* group, Vector3* offset, i32 flags)
{
    InitTexSheet();

    char full_name[256];
    arts_strcpy(full_name, name);

    if (group)
    {
        arts_strcat(full_name, "/");
        arts_strcat(full_name, group);
    }

    MeshCurrentObject = full_name;

    // Try exact match, then LOD suffixes (_H, _M, _L, _VL)
    static const char* const lod_suffixes[] = {"", "_H", "_M", "_L", "_VL"};

    char bms_path[256];
    Owner<Stream> stream;

    for (usize si = 0; si < ARTS_SIZE(lod_suffixes); ++si)
    {
        const char* suffix = lod_suffixes[si];

        if (group)
            arts_sprintf(bms_path, "bms/%s/%s%s.bms", name, group, suffix);
        else
            arts_sprintf(bms_path, "bms/%s%s.bms", name, suffix);

        stream = FileSystem::OpenAny(bms_path, true, nullptr, 0);

        if (stream)
        {
            if (suffix[0] != '\0')
            {
                arts_strcat(full_name, suffix);
                MeshCurrentObject = full_name;
            }
            break;
        }
    }

    if (!stream)
        return nullptr;

    u32 magic = 0;
    stream->Read(&magic, sizeof(magic));

    if (magic != 0x4D534833)
    {
        Warningf("GetMeshSet: %s has invalid magic", bms_path);
        return nullptr;
    }

    Vector3 bounds;
    stream->Read(&bounds, sizeof(bounds));

    if (offset)
    {
        f32 dx = offset->x - bounds.x;
        f32 dy = offset->y - bounds.y;
        f32 dz = offset->z - bounds.z;

        if (dx * dx + dy * dy + dz * dz > 1e-4f)
            return nullptr;
    }

    agiMeshSet* mesh = new agiMeshSet();

    if (flags & MESH_SET_VARIANT_MASK)
        mesh->Variant = (flags >> MESH_SET_VARIANT_SHIFT) & 0xFF;

    mesh->BinaryLoad(stream.get());
    mesh->Resident = 2;

    if (offset)
        mesh->Offset(*offset);

    ++MeshesPaged;

    return mesh;
}
