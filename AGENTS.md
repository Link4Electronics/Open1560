# Open1560 — Agent Context

## AR File Debugging Tools
Python scripts for inspecting AR archives (located in project root):
- **`parse_ar.py`** — Lists all files in AR archives with pixel dimensions for BMF/DDS/JPG/PNG/TGA/BMP. Shows button bitmap paths and dimensions. Usage: `./parse_ar.py`
- **`debug_ar.py`** — Dumps raw directory structure of an AR file (node entries, offsets, sizes, names). Good for understanding the VFS layout.
- AR format: u32 magic(`0x53455241 "ARES"`) + u32 node_count + u32 root_count + u32 names_size + node[count] + names_data + file_data. Each node is 12 bytes: u32 offset/u32 size_flags/u32 name_flags.
- File names can embed `\x01` (integer placeholder filled from name_int field) and extensions from a separate extension table.

### Ownership rules for Ptr<T> + AdoptChild
- `Ptr<T>` is `unique_ptr<T>`. When calling `AdoptChild(Ptr<asNode>(std::move(ptr)))`, ownership transfers to the child list.
- **NEVER** do `AdoptChild(Ptr<asNode>(&*ptr))` — this creates a second unique_ptr to the same object, causing double-free.
- Pattern: keep a raw pointer for later access, move the unique_ptr into AdoptChild:
  ```cpp
  Ptr<mmDropDown> dd = arnew mmDropDown();
  dd->Init(...);
  raw_ptr_ = dd.get();          // raw pointer for later access
  AdoptChild(Ptr<asNode>(std::move(dd))); // ownership transferred
  ```

## Project
Reverse-engineering of Midtown Madness 1 Beta. C++ + x86 assembly (game.asm) + SDL3 + OpenGL. CMake build targeting Linux.

## Build & Test
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```
Requires data files from original MM1 Beta (mm1beta.ain, data.AR, etc.). Run from directory containing data.

Optional: FFmpeg libraries (libavformat, libavcodec, libswscale) for intro video playback.

## Architecture

### game.asm (200k+ lines MASM)
- Original game logic in x86 assembly. **NOT linked on Linux**.
- All symbols are `ARTS_IMPORT` — imported from the original Windows binary at runtime in the original build.
- On Linux standalone build, these symbols must come from C++ reimplementations.

### Stub system
Three layers provide fallbacks on Linux:

1. **game_stubs.cpp** — Weak `__attribute__((weak))` C++ stubs (~200+ functions). No-op/zero return. Overridden by strong C++ definitions at link time (GCC/Clang).

2. **game_stubs.S** — Weak assembly stubs for data symbols (margins, constants) and some jump targets. Some sizes are wrong (e.g., `UI_LEFT_MARGIN` declared as 8 bytes instead of 4).

3. **Strong C++ implementations** — Override the weak stubs via normal linkage rules.

### Critical: functions with ONLY weak stubs
These are no-ops on Linux and break their respective features:

| Function | File | Impact |
|---|---|---|
| `VehicleSelectBase::AllSetCar()` | vselect.h | Car selection logic |
| `VehicleSelectBase::InitCarSelection()` | vselect.h | Vehicle UI initialization |
| `VehicleSelectBase::IncCar()` | vselect.h | Next/prev car navigation |
| `UIMenu::Update()` (weak) | game_stubs.cpp:488 | General menu updates |

### Mouse → UI pipeline
1. SDL window event → `sdlevent.cpp` (convert to 640x480 game-space via `g_ViewportX/Y/WH`)
2. `event.cpp` (normalize 0-1 via `center_x_*2`)
3. `menu.cpp` `CheckInput()` (convert to menu-local via `menu_x_/y_/width_/height_`)
4. `MenuManager::MouseAction()` → `UIMenu::MouseHitCheck()` (compare with widget MinX/MaxX/MinY/MaxY)

### Menu layout
- `UIMenu` constructor sets defaults: `menu_x_=0.114f, menu_y_=0.07f, menu_width_=0.775f, menu_height_=0.855f`
- `UI_LEFT_MARGIN` set to `0.0f` in menu.cpp:33 (game.asm has `0.078125f`, but game.asm reconstruction may not be 100% accurate for UI positions)
- `main.cpp` buttons positioned using `UI_LEFT_MARGIN`

### Row convention for bitmaps vs. text
- **Loaded bitmaps** (BMF/JPG from AR archives) store row 0 = bottom of image (OpenGL convention).
- **Text bitmaps** from FreeType render row 0 = top of image (DirectX/FreeType convention).
- `CopyBitmap`/`StretchCopyBitmap` swap V texcoords (`std::swap(tv_low, tv_high)`) to correct for GL convention.
- `mmTextNode::Cull()` flips the rendered text surface vertically after `RenderText()` and before `SafeBeginGfx()` to convert FreeType row order to GL row order.
- Net result: both loaded images and text appear right-side up.

### NavBar visibility
- `EnableNavBar`/`DisableNavBar` are `ARTS_IMPORT` (declared in `manager.h:92,101`). Implemented in `manager.cpp`:
  - `DisableNavBar()`: calls `nav_bar_->DeactivateNode()` (clears `NODE_FLAG_ACTIVE` bit 0 → stops rendering).
  - `EnableNavBar()`: calls `nav_bar_->ActivateNode()` then `nav_bar_->TurnOnPrev()`.
- Sub-menus (About, Audio, Graphics, Controls) should call `DisableNavBar` when active and `EnableNavBar` when leaving.
- The `uiNavBar` is a `UIMenu` overlay (menu_id=0) with buttons at absolute screen coordinates:
  - `mnav_opt` at (0.72, 0.0), `mnav_help` at (0.80, 0.0), `mnav_stow` at (0.88, 0.0), `mnav_exit` at (0.96, 0.0)
  - `mnav_prev` at (0.0, 0.9) — hidden via `SetPrevPos(0,0)` on sub-menus
  - NavBar always has `ActivateNode()` in its constructor; only `DisableNavBar` stops it.

### UIIcon widget
- `UIIcon` loads a bitmap at given pixel position and renders it via `CopyBitmap`.
- `Init(name, x, y)` converts float widget coords to pixel `dst_x_ = (int)(pipe_width * x)`, `dst_y_ = (int)(pipe_height * y)`; calls `Pipe()->GetBitmap(name, 0,0,1)` to load; creates 50×50 dummy bitmap fallback if not found.
- Lifecycle: `LoadBitmap()` in Init → `Update()` declares bitmap via `CullMgr()->DeclareBitmap(this, bitmap_.get())` → `Cull()` renders `Pipe()->CopyBitmap(dst_x_, dst_y_, bitmap_, 0, 0, width, height)`.
- `GetHitArea()` returns ratio of bitmap dimensions to pipe dimensions for hit testing.

### UITextDropdown + mmDropDown (expanded list)
- `drop_arrow` bitmap has 3 frames stacked vertically: normal (0), hover/active (1), expanded (2). Loaded in `UITextDropdown::Init()` from `ui.ar`.
- `mmDropDown` manages per-item text nodes. `InitString(options)` creates `mmTextNode` per pipe-separated option at `(X, Bottom + i*Height)`. `SetHighlight(i)` toggles BORDER+HIGHLIGHT effects on the highlighted item.
- `SetSliderFocus(1)` expands: sets `expanded_=1`, updates `MaxY` hit bounds to include dropdown list height, enables mmDropDown rendering. `SetSliderFocus(0)` collapses.
- `Action()` on click: when not expanded → `SetSliderFocus(1)` (expand); when expanded → compute hit index from `(mouseY - Bottom) / Height`, select item → `SetSliderFocus(0)` (collapse).
- `CaptureAction()` updates highlight on mouse move over expanded list via `mmDropDown::GetHit`.
- Keyboard: `EQ_VK_DOWN`/`RIGHT` cycles down, `EQ_VK_UP`/`LEFT` cycles up, `EQ_VK_RETURN`/`SPACE` toggles expand/collapse.

### About screen in game.asm
- Original `AboutMenu` constructor (`game.asm:206689-206816`):
  - Background bitmap: `"credits"` (PATCH: uses credits image instead of original)
  - Hotspot: `"Credits"` at (0.1, 0.1, 0.5, 0.5)
  - Done button: `"onav_done"` at (x=0.2, y=0.9) with div_type=4
  - Product ID label: `"PID Label"` at (x=0.203125, y=0.2708333, w=0.15625, h=0.0375, font=20)
- `PreSetup`: sets `prev_menu_id_ = 0` (no back navigation), stores simulation timestamp
- `Update`: checks elapsed time (auto-dismiss after timeout?)
- No explicit `DisableNavBar` call in AboutMenu (handled externally in original game logic)

### Video player (`mmvid/videoplayer.cpp`)
- Uses FFmpeg (libavformat/libavcodec/libswscale) to decode Intel Indeo 5 (IV50) AVI
- Renders via a temporary `SDL_Renderer` before OpenGL pipeline init
- **Letterboxed** via `SDL_SetRenderLogicalPresentation(…, SDL_LOGICAL_PRESENTATION_LETTERBOX)`
- **Audio**: Decoded through FFmpeg audio codec; auto-detects PCM format (U8/S16/S32/F32), mono/stereo, any sample rate
- Skip: any keypress, mouse click, joystick button, or quit event
- Compile-time optional: define `ARTS_HAVE_FFMPEG` is set when FFmpeg found via pkg-config

## Known Issues
- **Buttons misplaced**: `UI_LEFT_MARGIN` kept at `0.0f` (menu.cpp:33) even though game.asm has `0.078125f`. The game.asm reconstruction may not be 100% accurate — button positions were tuned empirically from a 1376×768 screenshot rather than using the game.asm constants. If adjusting positions, use the coordinate pipeline: widget → ScaleWidget(menu_x/y/w/h) → screen_norm → pixel.
- **game_stubs.S size errors**: `UI_LEFT_MARGIN`, `UI_LEFT_MARGIN2` declared 8 bytes instead of 4 (`.long` → 4, `.quad` → 8 mismatch)
- **Many game features** rely on weak stubs (physics, AI, audio, network, etc.) and will not work until reimplemented
- **Vehicle selection screen** is a placeholder: shows `veh_back` background, no car selection logic or 3D showcase
- **Options sub-menus are placeholders**: Audio, Graphics, Controls, and About menus are simple UIMenu subclasses with only a "Done" button. Full implementations require reimplementing AudioOptions, GraphicsOptions, ControlSetup, and AboutMenu (all ARTS_IMPORT from game binary).
- **VehicleSelectBase and Vehicle** have minimal implementations (constructors, PreSetup/PostSetup stubs) — most methods (InitCarSelection, IncCar, DecCar, SetPick, etc.) are weak no-ops
- **Intro video** plays only if FFmpeg dev libraries are installed at build time. Probes `game/logos.avi` then `logos.avi` to handle different CWDs.
- **Font fallback** improved: adds Comic Sans MS font name mapping; searches `FONT/` and `fonts/` dirs with case fallback; tries Linux system fonts as last resort. Unknown font names fall back to Gill Sans MT.

### Text rendering pipeline
- `mmTextNode::AddText()` — was a weak stub returning 0. Now stores font/text/position/effects in `lines_[line_count_++]`, sets `touched_ = true`, returns the line index.
- `mmTextNode::Update()` — was a weak stub doing nothing. Now calls `asNode::Update()` then `CullMgr()->DeclareBitmap(this, text_bitmap_)` to register the text node for rendering.
- `PUMenuBase::Update()` — was a weak stub doing nothing. Now calls `CullMgr()->DeclareBitmap(this, bg_bitmap_)` then `UIMenu::Update()`.

### Video player audio
- Added `SDL_InitSubSystem(SDL_INIT_AUDIO)` before setting up the audio stream, since the video plays before the main menu (which normally initializes audio).
- Replaced raw-packet audio feed with FFmpeg audio codec decode + format auto-detection (U8/S16/S32/F32 float, any channels/rate).

### Vehicle/VehShowcase menus
- Added bitmap buttons to Vehicle menu: Back (`onav_done`), Drive (`vehi_play`), Showcase (`vehi_show`), Auto (`veh_auto`).
- Added interface dispatch for Vehicle menu button IDs (Back → Main, Showcase → IDM_SHOWCASE).
- Removed broken `Showcases.SubString(CurrentCar+1)` background in VehShowcase -- now uses `"veh_back"` directly.
- Added VehShowcase dispatch (returns to Vehicle menu).

### 3D Vehicle Preview — Pipeline Status
The vehicle selection screen (`IDM_VEHICLE`, `veh_back`) 3D preview. Most C++ components are now strong implementations:

| Layer | Component | Status | File |
|---|---|---|---|
| 1 | `mmVehInfo::Load(char*)` | ✅ Strong impl | `vehinfo.cpp:34` |
| 2 | `mmVehInfo::mmVehInfo()` | ✅ Strong impl | `vehinfo.cpp:29` |
| 3 | `mmVehicleForm::SetShape()` | ✅ Strong impl | `vehform.cpp:73` |
| 4 | `mmVehicleForm::Cull()` | ✅ Strong impl | `vehform.cpp:81` |
| 5 | `GetMeshSet()` | ✅ Strong impl | `getmesh.cpp:38` |
| 6 | `TEXSHEET.Load(const char*)` | ✅ Strong impl | `texsheet.cpp:96` |
| 7 | `TEXSHEET.Lookup/GetVariationCount` | ✅ Strong impl | `texsheet.cpp:246-325` |
| 8 | `VehicleSelectBase::SetPick()` | ✅ Strong impl | `vselect.cpp:374` |
| 9 | `VehicleSelectBase::Update()` | ✅ Renders camera+mesh | `vselect.cpp:139` |

**core.ar** (91 MB, loaded first via `mods.txt`) contains all vehicle data:
- **BMS/** — Vehicle meshes with LOD suffixes (`_H`, `_M`, `_L`, `_VL`): `VPBUG/BODY_H.BMS`, `VPBUG/SHADOW_H.BMS`, `VPBUG/WHL0_H.BMS`, etc. Also building backdrop meshes.
- **MTL/** — Texture sheets: `GLOBAL.TSH` (global), `VPBUG.TSH`, `VPSEMI.TSH`, etc. + `MATERIAL.DB`, `PHYSICS.DB`, `TEXTURE.DB`
- **TEX16A/** / **TEX16O/** / **TEXP/** — Vehicle texture DDS files (for OpenGL and software renderers)
- **TUNE/** — Vehicle `.INFO` files (BEETLE, BUS, CADDI, CHICAGO, COP, F350, FASTBACK, MUSTANG99, PANOZGT, ROADSTER, SEMI) and CSV tune files

**Limiting factors for 3D preview:**
1. `GetMeshSet()` in `getmesh.cpp` was missing LOD suffix fallback. Vehicle BMS files use `_H`/`_M`/`_L`/`_VL` suffixes (e.g. `BODY_H.BMS`), but `GetMeshSet` was looking for `BODY.BMS` (no suffix). **Fixed**: now tries exact name, then `_H`, `_M`, `_L`, `_VL` in order (`getmesh.cpp:53-82`).
2. Sound files — `audio.ar`

**Vehicle data flow (for reference when data is available):**
- `.info` files: `tune/*.info` (e.g. `tune/BEETLE.INFO`)
- Format: `BaseName=vpbug`, `Description=...`, `Colors=...`, `Flags=%d`, `Order=%d`, `ScoringBias=%f`, `Horsepower=%d`, `Top Speed=%d`, `Durability=%d`, `Mass=%d`
- Default vehicle: `"vpbug"` (VW New Beetle)
- Geometry: `bms/<name>[/<group>].bms` loaded via `GetMeshSet(name, group, offset, flags)`
- Textures: `mtl/<name>.tsh` and `mtl/global.tsh` loaded via TEXSHEET
- BMS file format: `u32 magic(0x4D534833 "MSH3")` + `Vector3 bounds` + BinaryLoad data

## Previously Fixed
- `mmInterface::SetNavigationOrders()` — added to interface.cpp for widget tab ordering
- `mmInterface::ShowMain()`, `Reset()`, `Update()` — real implementations
- `UIMenu::SetFocusWidget()`, `SetSelected()` — added strong implementations
- `MenuManager::GetFont(i32)` — added lazy font initialization
- **UIIcon implementation**: Rewrote `icon.h` with real members (`dst_x_`, `dst_y_`, `Rc<agiBitmap> bitmap_`). `icon.cpp` has full Init/LoadBitmap/Cull/GetHitArea/CreateDummyBitmap/Switch/Update.
- **UITextDropdown expanded list**: Added `bitmap_` drop_arrow rendering (3 frames), `expanded_` flag, `SetSliderFocus()` toggle, `mmDropDown` child with per-item text nodes, hit-test selection in `Action()`, highlight tracking in `CaptureAction()`, keyboard navigation (arrows/enter/space).
- **mmDropDown implementation**: Implemented all missing methods (Init, SetString, Update, GetHit, FindFirstEnabled, GetCurrentString, SetDisabledColors). Items rendered as child `mmTextNode`s with MM_TEXT_VCENTER/PADDING/REQUIRED effects.
- **Driver menu selection**: Added `AddTextDropdown` player selector with expanded popup, prev/next buttons for keyboard navigation, info panel showing rank/last race/vehicle/controller/netname/score, `IncPlayer()`/`DecPlayer()`/`SetPlayerPick()` for player management.
- **Ptr<T> + AdoptChild bug**: Documented that `AdoptChild(Ptr<asNode>(&*ptr))` creates double-ownership (both the local Ptr<unique_ptr> and AdoptChild's child list try to own the object). Use raw pointer + `std::move(ptr)` instead.
- **Options screen navigation**: Fixed `UIMenu::Enable()` crash when `focus_widget_index_ == -1` (caused by `SetFocusWidget(-1)` in OptionsMenu ctor after SetFocusWidget got a real implementation). Added dispatch handlers in `mmInterface::Update()` for OptionsMenu buttons (Audio/Graphics/Controls → sub-menus, Credits → About). Removed `SetFocusWidget(-1)` from OptionsMenu constructor. Created placeholder sub-option menus registered in `midtown.cpp`.
- **SwitchNow crash**: Added null check for `GetMenu(id)` in `SwitchNow()` to prevent crash when switching to a non-existent menu (e.g., Quick Race before Vehicle menu was registered)
- **Vehicle/VehShowcase creation**: Added constructor implementations for `Vehicle`, `VehicleSelectBase`, and `VehShowcase`. Created and registered Vehicle (IDM_VEHICLE) and VehShowcase (IDM_SHOWCASE) menus in `midown.cpp`. 
- **Intro video**: Implemented `PlayIntroVideo()` using FFmpeg + SDL renderer. Plays before OpenGL pipeline init. Skippable via keypress/mouse click. Probes multiple paths (`game/logos.avi`, `logos.avi`).
- **Video letterbox + audio decode**: Added `SDL_SetRenderLogicalPresentation` for aspect-ratio-correct playback. Replaced raw-packet audio feed with FFmpeg audio codec decode + auto-format detection (U8/S16/S32/F32, any channels/rate).
- **KeyboardAction/MouseAction null checks**: Added null-pointer guards in `UIMenu::KeyboardAction()` and `UIMenu::MouseAction()` to prevent segfault on menus with no widgets (e.g., placeholder Vehicle menu).
- **Font TTF names fixed**: 1560.ar stores fonts as `FONT/GIL_____`, `FONT/GILB____`, `FONT/BROADW` (no `.TTF` extension). Code was looking for `GIL_____.TTF` etc., causing VFS lookup to fail and `mmFont::Create()` to return null → all text silent. Removed `.TTF` from hardcoded names in `mmtext_freetype.cpp:478-493`, added fallback that tries appending `.TTF` for host filesystem compatibility.
- **Text surface flip**: Added vertical row-flip in `mmTextNode::Cull()` after `RenderText()` and before `SafeBeginGfx()` (`mmtext.cpp:69-86`). FreeType renders row 0 = top-of-text, but the GL pipeline expects row 0 = bottom-of-image (same as loaded BMF/JPG). The flip + CopyBitmap's V texcoord swap gives correct text.
- **CopyBitmap swap universal**: Removed `is_wayland` guard — `std::swap(tv_low, tv_high)` now always applied in both `CopyBitmap` and `StretchCopyBitmap` (`glpipe.cpp`). Loaded AR images store row 0 = bottom (GL convention).
- **NavBar hiding on About**: Implemented `MenuManager::DisableNavBar()`/`EnableNavBar()` via `DeactivateNode()`/`ActivateNode()` + `TurnOnPrev()` (`manager.cpp:619-632`). About screen calls `DisableNavBar()` in `PreSetup()`, `EnableNavBar()` in `PostSetup()`.
- **Done button position**: Fixed to `(0.2f, 0.9f)` with `div_type=4` matching original game.asm (`placeholder_opts.cpp:109`).
- **Product ID text position**: Adjusted to `(0.27f, 0.30f, 0.12f, 0.032f)` matching original menu-local `(0.203125, 0.2708333, 0.15625, 0.0375)` converted to screen-space (`placeholder_opts.cpp:114`).
- **Scrolling credits on About screen**: Loads `ABOUT_CRED` (ui.ar) or fallback `CREDITS` bitmap. Scrolls at 50 px/s after 1.5s delay. Overrides `Update()` (computes scroll, calls `CullMgr()->DeclareBitmap`) and `Cull()` (renders wrapped CopyBitmap in two parts if scrolled past end). Positioned at screen `(0.1*w, 0.1*h)` with visible height `0.5*h` matching original hotspot `(0.1, 0.1, 0.5, 0.5)`.
- **About credits position + direction**: Fixed position to use screen-normalized coords after ScaleWidget `(0.1915, 0.1555, 0.4275)`. Reversed scroll direction: scrolls DOWN instead of UP (renders at `src_y = height - scroll - vis_h` to move window downward through image).
- **Vehicle list init**: Added `mmVehList` creation + `LoadAll()` call in `mmInterface::Reset()` (`interface.cpp:231`). Was missing — original called it from game.asm startup, so `NumVehicles=0` → 3D preview never rendered.
- **64-bit gap90 overlap fix**: `vselect.h` originally stored pointers inside `gap90[0xD8]` at 32-bit offsets (0x4C for forms, 0x50 for topSpeed, 0x54 for extra). On 64-bit, 8-byte pointer writes into adjacent 4-byte slots overlapped (forms at gap90[0x4C] wrote 8 bytes through 0x53, corrupting topSpeed at 0x50). Moved all pointers to proper member variables (`dofcs_array_`, `forms_array_`, `top_speed_array_`, `extra_array_`) plus int accessors (`current_car_`, `current_color_`, `vehicle_count_`) outside gap90 (`vselect.h:87-96`).
- **GetMeshSet LOD suffix fallback**: Vehicle BMS files use LOD suffixes (`_H`, `_M`, `_L`, `_VL`) on the group name (e.g. `BODY_H.BMS` instead of `BODY.BMS`). `GetMeshSet()` now tries the exact name first, then falls back to `_H`, `_M`, `_L`, `_VL` in order (`getmesh.cpp:53-82`). This allows the vehicle preview to load the correct mesh.

## Big-Endian Support Notes (On Hold)

If porting to big-endian platforms (e.g., PowerPC, SPARC, MIPS), the following areas need endian-awareness:

### Binary file formats (all little-endian in original)
- **BMS mesh files** (`bms/*.bms`): Header has `u32 magic(0x4D534833)` + `Vector3 bounds` + BinaryLoad vertex/index data. All multi-byte values need `le32toh`/`le16toh` etc.
- **TSH texture sheet** (`mtl/*.tsh`): Text-based (ASCII), no endian issues.
- **AR archives** (`*.ar`): File format uses little-endian u32 for entry counts and offsets. See `stream/farch.cpp`.
- **DDS textures** (`*.dds`): DDS header is little-endian. Use `SDL_iostream` or similar for portable reads.
- **BMF images** (`*.bmf`): Custom format — check `agisw/swbitmap.cpp` for pixel data access.

### Platform assumptions
- **`#pragma pack`**: Used extensively for struct layouts matching original 32-bit Windows. May cause issues on some BE compilers.
- **`sizeof(void*)`**: 64-bit Linux assumes 8-byte pointers. All struct sizes verified with `check_size()` macros.
- **x86 assembly stubs**: `game_stubs.S` has x86-64 assembly (`xor eax,eax` / `ret`). On non-x86, these must be replaced with C++ weak stubs or `#ifdef` guards.

### Rendering / pixel data
- **`u32` colors**: Stored as `0xAABBGGRR` (Windows GDI convention). On BE, byte-order reversal needed when reading/writing pixel data.
- **DXT compressed textures**: Endian-independent at block level (S3TC), but mip chain sizes in headers are LE.

### Steps to enable BE build
1. Add `#include <endian.h>` and wrap all `magic`/multi-byte reads with `le32toh()`.
2. Add `-DWORDS_BIGENDIAN` to CMake for BE targets, guard byte-swap code.
3. Replace `game_stubs.S` x86 assembly with C++ weak stubs (most are already duplicated in `game_stubs.cpp`).
4. Test with a BE emulator (QEMU user-mode for PowerPC/MIPS) or cross-compile toolchain.
