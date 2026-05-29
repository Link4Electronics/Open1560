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
| `UIMenu::SetFocusWidget(i32)` | game_stubs.cpp:483 | Menu focus/navigation |
| `UIMenu::SetSelected()` | game_stubs.cpp:484 | Widget selection state |
| `MenuManager::GetFont(i32)` | game_stubs.cpp:449 | Text rendering |

### Mouse → UI pipeline
1. SDL window event → `sdlevent.cpp` (convert to 640x480 game-space via `g_ViewportX/Y/WH`)
2. `event.cpp` (normalize 0-1 via `center_x_*2`)
3. `menu.cpp` `CheckInput()` (convert to menu-local via `menu_x_/y_/width_/height_`)
4. `MenuManager::MouseAction()` → `UIMenu::MouseHitCheck()` (compare with widget MinX/MaxX/MinY/MaxY)

### Menu layout
- `UIMenu` constructor sets defaults: `menu_x_=0.114f, menu_y_=0.07f, menu_width_=0.775f, menu_height_=0.855f`
- `UI_LEFT_MARGIN` overridden to `0.0f` in menu.cpp:33 (original in game.asm was `0.078125f`)
- `main.cpp` buttons positioned using `UI_LEFT_MARGIN` shifted to left edge

## Known Issues
- **Buttons misplaced**: `UI_LEFT_MARGIN` set to 0.0f vs original 0.078125f
- **Navigation broken**: `SetFocusWidget` and `SetSelected` were weak no-ops (now fixed with strong implementations)
- **game_stubs.S size errors**: `UI_LEFT_MARGIN`, `UI_LEFT_MARGIN2` declared 8 bytes instead of 4 (`.long` → 4, `.quad` → 8 mismatch)
- **Fonts not initialized in MenuManager ctor** (purpose of original game.asm code). `GetFont` now lazy-initializes via `mmText::CreateFont`.
- **Many game features** rely on weak stubs (physics, AI, audio, network, etc.) and will not work until reimplemented

## Previously Fixed
- `mmInterface::SetNavigationOrders()` — added to interface.cpp for widget tab ordering
- `mmInterface::ShowMain()`, `Reset()`, `Update()` — real implementations
- `UIMenu::SetFocusWidget()`, `SetSelected()` — added strong implementations
- `MenuManager::GetFont(i32)` — added lazy font initialization
