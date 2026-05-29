# Open1560 — Agent Context

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
- `UI_LEFT_MARGIN` overridden to `0.0f` in menu.cpp:33 (original in game.asm was `0.078125f`)
- `main.cpp` buttons positioned using `UI_LEFT_MARGIN` shifted to left edge

### Video player (`mmvid/videoplayer.cpp`)
- Uses FFmpeg (libavformat/libavcodec/libswscale) to decode Intel Indeo 5 (IV50) AVI
- Renders via a temporary `SDL_Renderer` before OpenGL pipeline init
- Audio: PCM u8 played through SDL audio stream
- Skip: any keypress, mouse click, joystick button, or quit event
- Compile-time optional: define `ARTS_HAVE_FFMPEG` is set when FFmpeg found via pkg-config

## Known Issues
- **Buttons misplaced**: `UI_LEFT_MARGIN` set to 0.0f vs original 0.078125f
- **game_stubs.S size errors**: `UI_LEFT_MARGIN`, `UI_LEFT_MARGIN2` declared 8 bytes instead of 4 (`.long` → 4, `.quad` → 8 mismatch)
- **Many game features** rely on weak stubs (physics, AI, audio, network, etc.) and will not work until reimplemented
- **Vehicle selection screen** is a placeholder: shows `veh_back` background, no car selection logic or 3D showcase
- **Options sub-menus are placeholders**: Audio, Graphics, Controls, and About menus are simple UIMenu subclasses with only a "Done" button. Full implementations require reimplementing AudioOptions, GraphicsOptions, ControlSetup, and AboutMenu (all ARTS_IMPORT from game binary).
- **VehicleSelectBase and Vehicle** have minimal implementations (constructors, PreSetup/PostSetup stubs) — most methods (InitCarSelection, IncCar, DecCar, SetPick, etc.) are weak no-ops
- **Intro video** plays only if FFmpeg dev libraries are installed at build time. Probes `game/logos.avi` then `logos.avi` to handle different CWDs.
- **Font fallback** improved: adds Comic Sans MS font name mapping; searches `FONT/` and `fonts/` dirs with case fallback; tries Linux system fonts as last resort. Unknown font names fall back to Gill Sans MT.

## Previously Fixed
- `mmInterface::SetNavigationOrders()` — added to interface.cpp for widget tab ordering
- `mmInterface::ShowMain()`, `Reset()`, `Update()` — real implementations
- `UIMenu::SetFocusWidget()`, `SetSelected()` — added strong implementations
- `MenuManager::GetFont(i32)` — added lazy font initialization
- **Options screen navigation**: Fixed `UIMenu::Enable()` crash when `focus_widget_index_ == -1` (caused by `SetFocusWidget(-1)` in OptionsMenu ctor after SetFocusWidget got a real implementation). Added dispatch handlers in `mmInterface::Update()` for OptionsMenu buttons (Audio/Graphics/Controls → sub-menus, Credits → About). Removed `SetFocusWidget(-1)` from OptionsMenu constructor. Created placeholder sub-option menus registered in `midtown.cpp`.
- **SwitchNow crash**: Added null check for `GetMenu(id)` in `SwitchNow()` to prevent crash when switching to a non-existent menu (e.g., Quick Race before Vehicle menu was registered)
- **Vehicle/VehShowcase creation**: Added constructor implementations for `Vehicle`, `VehicleSelectBase`, and `VehShowcase`. Created and registered Vehicle (IDM_VEHICLE) and VehShowcase (IDM_SHOWCASE) menus in `midown.cpp`. 
- **Intro video**: Implemented `PlayIntroVideo()` using FFmpeg + SDL renderer. Plays before OpenGL pipeline init. Skippable via keypress/mouse click. Probes multiple paths (`game/logos.avi`, `logos.avi`).
