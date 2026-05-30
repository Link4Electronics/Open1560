#!/usr/bin/env python3
"""Debug: dump first bytes of DDS files from AR archive."""

import struct
import os

filepath = "/home/acer/Downloads/Open1560/game/ui.ar"

with open(filepath, "rb") as f:
    magic = struct.unpack("<I", f.read(4))[0]
    node_count, root_count, names_size = struct.unpack("<III", f.read(12))
    print(f"node_count={node_count}, root_count={root_count}, names_size={names_size}")

    nodes = []
    for i in range(node_count):
        field_0, field_4, field_8 = struct.unpack("<III", f.read(12))
        nodes.append((field_0, field_4, field_8))

    names_data = f.read(names_size)
    files_data_offset = f.tell()
    print(f"files_data_offset = {files_data_offset}")

# Find MNAV_EXIT
for i, (field_0, field_4, field_8) in enumerate(nodes):
    name_offset = (field_8 >> 14) & 0x3FFFF
    ext_offset = (field_4 >> 23) & 0x1FF
    name_int = (field_8 >> 1) & 0x1FFF
    is_dir = bool(field_8 & 1)
    
    raw = names_data[name_offset:].split(b"\0")[0].decode("latin-1", errors="replace")
    name_str = raw.replace('\x01', f'\\x01(INT={name_int})')
    
    if is_dir:
        entry_idx = field_0
        entry_count = field_4 & 0x7FFFFF
        print(f"  [{i:4d}] DIR  name='{name_str}'  entry_idx={entry_idx}  count={entry_count}  ext_off={ext_offset}")
    else:
        offset = field_0
        size = field_4 & 0x7FFFFF
        print(f"  [{i:4d}] FILE name='{name_str}'  offset={offset}  size={size}  ext_off={ext_offset}")

# Find MNAV_EXIT nodes
for i, (field_0, field_4, field_8) in enumerate(nodes):
    name_offset = (field_8 >> 14) & 0x3FFFF
    raw = names_data[name_offset:].split(b"\0")[0].decode("latin-1", errors="replace")
    
    if "MNAV_EXIT" in raw or "MNAV_OPT" in raw or "MNAV_HELP" in raw:
        offset = field_0
        size = field_4 & 0x7FFFFF
        print(f"\n--- File at index {i} ---")
        print(f"  raw name: {raw!r}")
        print(f"  field_0={field_0:#x}, field_4={field_4:#x}, field_8={field_8:#x}")
        print(f"  offset={offset}, size={size}, is_dir={bool(field_8&1)}")
        ext_off = (field_4 >> 23) & 0x1FF
        print(f"  name_off={(field_8 >> 14) & 0x3FFFF}, ext_off={ext_off}")
        if ext_off:
            ext = names_data[ext_off:].split(b"\0")[0]
            print(f"  ext string: {ext}")
        
        # Read file data
        with open(filepath, "rb") as f:
            f.seek(files_data_offset + offset)
            data = f.read(min(size, 64))
            print(f"  First {len(data)} bytes: {data.hex()}")
            print(f"  As text: {data!r}")
