#!/usr/bin/env python3
"""Extract all files from Angel Studios AR archives with correct names/extensions."""

import struct
import os
import sys

EXT_TABLE = [
    "", "WAV", "", "REX", "", "BND", "DLP", "DDS",
    "", "ASF", "GLX", "INFO", "TXP", "TXD", "TXL", "TXA",
    "JPG", "TGA", "BMP", "PNG", "", "TCS", "BMF", "TCB",
    "TCD", "TCE", "TCF", "TCG", "TCH", "TCI", "TCJ", "TCK",
]

def get_ext(ext_off, names_data):
    if ext_off == 0:
        return ""
    if ext_off < len(EXT_TABLE) and EXT_TABLE[ext_off]:
        return "." + EXT_TABLE[ext_off]
    # For extensions outside the known table, read from names_data
    if ext_off < len(names_data):
        return "." + names_data[ext_off:].split(b"\0")[0].decode("latin-1", errors="replace")
    return f".ext{ext_off}"

def build_path_map(nodes, names_data):
    """Build a map from node index to full virtual path."""
    dirs = {}
    for i, (field_0, field_4, field_8) in enumerate(nodes):
        if bool(field_8 & 1):
            name_offset = (field_8 >> 14) & 0x3FFFF
            name_int = (field_8 >> 1) & 0x1FFF
            raw = names_data[name_offset:].split(b"\0")[0].decode("latin-1", errors="replace")
            name_str = raw.replace("\x01", str(name_int))
            entry_idx = field_0
            entry_count = field_4 & 0x7FFFFF
            dirs[i] = (name_str, entry_idx, entry_count)

    # For each file, find its parent dir (most specific nested one)
    def find_parent_path(node_idx):
        parts = []
        while True:
            best_di = None
            best_start = -1
            for di, (dname, dstart, dcount) in dirs.items():
                if dstart <= node_idx < dstart + dcount and node_idx != di:
                    if dstart > best_start:
                        best_start = dstart
                        best_di = di
            if best_di is None:
                break
            dname, _, _ = dirs[best_di]
            parts.insert(0, dname)
            node_idx = best_di
        return "/".join(parts)

    paths = {}
    for i in range(len(nodes)):
        if not bool(nodes[i][2] & 1):  # file, not dir
            paths[i] = find_parent_path(i)
    return paths, dirs

def unpack_ar(filepath, outdir):
    basename = os.path.basename(filepath)

    with open(filepath, "rb") as f:
        magic = struct.unpack("<I", f.read(4))[0]
        assert magic == 0x53455241, f"Bad magic: {magic:#x}"
        node_count, root_count, names_size = struct.unpack("<III", f.read(12))

        nodes = []
        for _ in range(node_count):
            field_0, field_4, field_8 = struct.unpack("<III", f.read(12))
            nodes.append((field_0, field_4, field_8))

        names_data = f.read(names_size)
        files_data_offset = f.tell()

    paths, dirs = build_path_map(nodes, names_data)
    print(f"{basename}: {node_count} entries, {len(dirs)} dirs, files at +{files_data_offset}")

    count = 0
    with open(filepath, "rb") as f:
        for i, (field_0, field_4, field_8) in enumerate(nodes):
            if bool(field_8 & 1):
                continue

            name_offset = (field_8 >> 14) & 0x3FFFF
            ext_offset = (field_4 >> 23) & 0x1FF
            name_int = (field_8 >> 1) & 0x1FFF
            size = field_4 & 0x7FFFFF

            raw = names_data[name_offset:].split(b"\0")[0].decode("latin-1", errors="replace")
            name_str = raw.replace("\x01", str(name_int))
            ext = get_ext(ext_offset, names_data)

            filename = name_str + ext
            parent_path = paths.get(i, "")
            full_path = os.path.join(outdir, parent_path, filename)
            os.makedirs(os.path.dirname(full_path), exist_ok=True)

            if os.path.exists(full_path):
                continue

            f.seek(files_data_offset + field_0)
            data = f.read(size)
            with open(full_path, "wb") as out:
                out.write(data)
            count += 1

    print(f"  Extracted {count} files")

def main():
    base = os.path.join(os.path.dirname(__file__), "game")
    out_base = os.path.join(os.path.dirname(__file__), "extracted")

    for name in ["1560.ar", "ui.ar", "core.ar", "audio.ar"]:
        path = os.path.join(base, name)
        if os.path.exists(path):
            outdir = os.path.join(out_base, name.replace(".ar", ""))
            print(f"\n=== {name} -> {outdir} ===")
            unpack_ar(path, outdir)

if __name__ == "__main__":
    main()
