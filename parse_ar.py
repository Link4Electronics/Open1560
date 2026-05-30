#!/usr/bin/env python3
"""Parse AR archives and find UI button bitmaps with their pixel dimensions."""

import struct
import os

AR_FILES = [
    "/home/acer/Downloads/Open1560/game/1560.ar",
    "/home/acer/Downloads/Open1560/game/ui.ar",
    "/home/acer/Downloads/Open1560/game/core.ar",
    "/home/acer/Downloads/Open1560/game/audio.ar",
]

BUTTON_NAMES = [
    "mnav_opt", "mnav_help", "mnav_stow", "mnav_exit", "mnav_prev",
    "onav_credit", "onav_aud", "onav_cont", "onav_gra", "onav_done",
]

BUTTON_PREFIXES = ["mnav_", "onav_"]


def parse_ar(filepath):
    """Parse an AR archive and return all entries + data offsets."""
    with open(filepath, "rb") as f:
        magic = struct.unpack("<I", f.read(4))[0]
        assert magic == 0x53455241, f"Bad magic in {filepath}"

        node_count, root_count, names_size = struct.unpack("<III", f.read(12))

        nodes = []
        for _ in range(node_count):
            field_0, field_4, field_8 = struct.unpack("<III", f.read(12))
            nodes.append((field_0, field_4, field_8))

        names_data = f.read(names_size)
        files_data_offset = f.tell()

    return node_count, root_count, nodes, names_data, files_data_offset


def expand_name(node, names_data):
    """Replicate VirtualFileSystem::ExpandName logic."""
    field_0, field_4, field_8 = node
    name_offset = (field_8 >> 14) & 0x3FFFF
    ext_offset = (field_4 >> 23) & 0x1FF
    name_int = (field_8 >> 1) & 0x1FFF  # GetNameInteger, max 8191

    # Read the raw name string (null-terminated)
    raw_name = names_data[name_offset:].split(b"\0")[0].decode("latin-1", errors="replace")

    # Process \x01 separators - replace with integer value
    result = []
    for c in raw_name:
        if c == '\x01':
            # Insert name_int value as decimal string
            val = name_int
            if val == 0 and not result:
                result.append('0')
            else:
                digits = []
                if val == 0:
                    digits.append('0')
                else:
                    while val:
                        digits.append(str(val % 10))
                        val //= 10
                result.extend(reversed(digits))
        else:
            result.append(c)
    name = ''.join(result)

    # Append extension if present
    if ext_offset:
        ext_bytes = names_data[ext_offset:].split(b"\0")[0]
        ext = ext_bytes.decode("latin-1", errors="replace")
        name += '.' + ext

    return name


def get_file_data(filepath, offset, size):
    """Read raw data from an AR archive at the given absolute offset."""
    with open(filepath, "rb") as f:
        f.seek(offset)
        return f.read(min(size, 256))


def get_dimensions(data, ext):
    """Try to extract image dimensions from raw bitmap data."""
    ext_lower = ext.lower()

    if ext_lower == ".bmf":
        if len(data) < 0x10:
            return None
        h = struct.unpack_from("<I", data, 0x08)[0]
        w = struct.unpack_from("<I", data, 0x0C)[0]
        return (w, h)

    elif ext_lower == ".dds":
        if len(data) < 20 or data[:4] != b"DDS ":
            return None
        h = struct.unpack_from("<I", data, 12)[0]
        w = struct.unpack_from("<I", data, 16)[0]
        return (w, h)

    elif ext_lower in (".jpg", ".jpeg"):
        if len(data) < 12 or data[:2] != b"\xff\xd8":
            return None
        i = 2
        while i < len(data) - 7:
            if data[i] == 0xFF:
                marker = data[i+1]
                if marker in (0xC0, 0xC2):
                    h = struct.unpack_from(">H", data, i+5)[0]
                    w = struct.unpack_from(">H", data, i+7)[0]
                    return (w, h)
                elif marker == 0xD9:
                    break
                elif marker == 0xDA:
                    break
                else:
                    if marker not in (0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9):
                        if i + 3 < len(data):
                            seg_len = struct.unpack_from(">H", data, i+2)[0]
                            i += seg_len + 2
                            continue
            i += 1
        return None

    elif ext_lower == ".png":
        if len(data) < 33 or data[:8] != b"\x89PNG\r\n\x1a\n":
            return None
        if data[12:16] != b"IHDR":
            return None
        w = struct.unpack_from(">I", data, 16)[0]
        h = struct.unpack_from(">I", data, 20)[0]
        return (w, h)

    elif ext_lower == ".tga":
        if len(data) < 18:
            return None
        w = struct.unpack_from("<H", data, 12)[0]
        h = struct.unpack_from("<H", data, 14)[0]
        return (w, h)

    elif ext_lower == ".bmp":
        if len(data) < 26 or data[:2] != b"BM":
            return None
        w = struct.unpack_from("<i", data, 18)[0]
        h = struct.unpack_from("<i", data, 22)[0]
        return (w, h)

    return None


def get_parent(entries, node_idx):
    """Find the directory entry that contains the given node index."""
    for pe in entries:
        if pe["is_dir"] and pe["entry_index"] <= node_idx < pe["entry_index"] + pe["entry_count"]:
            return pe
    return None


def build_paths(entries):
    """Build full virtual paths for all file entries."""
    file_paths = {}
    for e in entries:
        if not e["is_dir"]:
            parts = [e["name"]]
            idx = e["index"]
            for _ in range(50):
                parent = get_parent(entries, idx)
                if parent is None:
                    break
                parts.insert(0, parent["name"])
                idx = parent["index"]
            file_paths[e["index"]] = "/".join(parts)
    return file_paths


def list_all_image_files(ar_path):
    """List all image files in an archive with their dimensions."""
    node_count, root_count, nodes, names_data, _ = parse_ar(ar_path)

    entries = []
    for i, n in enumerate(nodes):
        name = expand_name(n, names_data)
        field_0, field_4, field_8 = n
        is_dir = bool(field_8 & 1)

        if is_dir:
            entries.append({
                "index": i, "name": name, "is_dir": True,
                "entry_index": field_0, "entry_count": field_4 & 0x7FFFFF,
            })
        else:
            entries.append({
                "index": i, "name": name, "is_dir": False,
                "offset": field_0, "size": field_4 & 0x7FFFFF,
            })

    file_paths = build_paths(entries)
    results = []
    image_exts = {'.bmf', '.dds', '.jpg', '.jpeg', '.png', '.tga', '.bmp'}

    for e in entries:
        if not e["is_dir"]:
            ext = os.path.splitext(e["name"])[1].lower()
            path = file_paths.get(e["index"], f"(index {e['index']})")
            dims = None
            if ext in image_exts:
                data = get_file_data(ar_path, e["offset"], e["size"])
                dims = get_dimensions(data, ext)

            entry = {
                "path": path,
                "name": e["name"],
                "ext": ext,
                "size": e["size"],
                "dims": dims,
            }
            results.append(entry)

    return results, root_count


def find_button_bitmaps(ar_path):
    """Find button bitmap files and return their details."""
    all_files, _ = list_all_image_files(ar_path)
    results = []

    for f in all_files:
        name_noext = os.path.splitext(f["name"])[0].lower()

        is_button = name_noext in [b.lower() for b in BUTTON_NAMES]
        if not is_button:
            is_button = any(name_noext.startswith(p) for p in BUTTON_PREFIXES)

        if is_button:
            dims = f["dims"]
            dim_str = f"{dims[0]}x{dims[1]}" if dims else "UNKNOWN"
            results.append({
                "archive": os.path.basename(ar_path),
                "path": f["path"],
                "size": f["size"],
                "dimensions": dim_str,
            })

    return results


def main():
    """Main entry point."""
    # Part 1: Find button bitmaps
    print("=" * 80)
    print("BUTTON BITMAPS")
    print("=" * 80)

    all_button_results = []
    for ar_path in AR_FILES:
        if not os.path.exists(ar_path):
            print(f"\n  [Missing: {ar_path}]")
            continue
        results = find_button_bitmaps(ar_path)
        all_button_results.extend(results)
        for r in results:
            print(f"\n  {r['archive']}")
            print(f"    Path:       {r['path']}")
            print(f"    Size:       {r['size']} bytes")
            print(f"    Dimensions: {r['dimensions']}")

    print("\n" + "=" * 80)
    print("SUMMARY - Button bitmap pixel dimensions")
    print("=" * 80)
    print(f"  {'Archive':10s} | {'Path':50s} | {'Dims':12s} | {'Size':>8s}")
    print("  " + "-" * 10 + "-+-" + "-" * 50 + "-+-" + "-" * 12 + "-+-" + "-" * 8)
    for r in sorted(all_button_results, key=lambda x: x["path"]):
        print(f"  {r['archive']:10s} | {r['path']:50s} | {r['dimensions']:12s} | {r['size']:>8d}")

    # Part 2: All image files with dimensions
    print("\n\n" + "=" * 80)
    print("ALL IMAGE FILES (>= 8x8)")
    print("=" * 80)

    for ar_path in AR_FILES:
        if not os.path.exists(ar_path):
            continue
        all_files, _ = list_all_image_files(ar_path)

        print(f"\n--- {os.path.basename(ar_path)} ---")
        for f in sorted(all_files, key=lambda x: x["path"]):
            if f["dims"] and f["dims"][0] and f["dims"][1]:
                    print(f"  {f['path']:55s} | {f['dims'][0]}x{f['dims'][1]:<8d} | {f['size']:>8d} bytes")
            else:
                # Show non-images that match UI-related keywords
                name_lower = f["name"].lower()
                if any(x in name_lower for x in ["nav", "onav", "button", "btn", "menu", "ui_", "back", "logo", "credits"]):
                    print(f"  {f['path']:55s} | {'?':12s} | {f['size']:>8d} bytes (unsupported format)")


if __name__ == "__main__":
    main()
