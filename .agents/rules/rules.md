# kiliseOS Development & Agent Guidelines

> **Project Target**: kiliseOS (Modern, minimalist operating system rebased from clean ReactOS upstream)  
> **Brand Identity**: Minimalist Geometric Globus Cruciger (♁)  
> **Design Language**: Material Design 3 (MD3 / ChromeOS-inspired surface & shelf) + Papirus Iconography  
> **Target Architecture**: x86 (i386 MinGW via RosBE)

---

## 1. Brand Identity & Visual Language

### 1.1 Brand Naming & Typography
- **Operating System Name**: `kiliseOS` (always written with lowercase `k` and uppercase `OS`; full product name: `kiliseOS Operating System`).
- **Organization / Publisher**: `kiliseOS Project`.
- **Product Name Macro**: `REACTOS_DEFAULT_STR_PRODUCT_NAME` in `sdk/include/reactos/version.rc` must strictly remain `"kiliseOS Operating System"`.
- **ISO Volume Label**: `ISO_VOLNAME` in `boot/boot_images.cmake` must remain `"kiliseOS"`.

### 1.2 Emblem & Geometry (Globus Cruciger ♁)
- All official branding, badges, bootscreen graphics, and system emblems follow the **Minimalist Geometric Globus Cruciger (Option 1)**:
  - **Main Orb**: Perfect circle outline.
  - **Equator**: Horizontal equator dividing line.
  - **Axis**: Vertical axis extending continuously from the bottom of the orb to the top cross spire.
  - **Crossbar**: Symmetrical horizontal bar mounted atop the vertical axis.
  - **Palette**: Deep Obsidian (`#131518` / `#191C1E`), Light Surface Container (`#212428`), Accent Cyan (`#8AB4F8`), and Crisp White (`#FFFFFF`).

### 1.3 Material Design 3 (MD3) & ChromeOS Shell Styling
- UI overhaul must prioritize:
  - Flat/subtle tonal elevations (elevation levels 1-3 via tonal color blending rather than heavy skeuomorphic bevels).
  - Rounded taskbar / floating shelf aesthetics for `traywnd.cpp` and `.msstyles`.
  - Consistent padding, smooth hover feedback, and high readability.

### 1.4 Iconography Standards (Papirus Theme)
- System icons (shell32, explorer, control panel applets, file types, devices, notifications) must strictly use **Papirus Icon Theme** SVGs converted to native format.
- Ensure vivid primary colors for desktop anchors (e.g. My Computer `#3584E4` blue chassis, vibrant displays, colorful applet glyphs).

---

## 2. Rebranding & Heritage Preservation Policy

### 2.1 Scrubbing User-Facing Artifacts ("Sạn")
- All user-visible dialogs, window captions, menus, wizards, toolbars, and system utilities must be thoroughly rebranded to **kiliseOS**:
  - **Logon / Logoff / Shutdown**: `msgina.dll`, `shell32.dll` (`reactos_flag.bmp`, dialog captions `Shut Down kiliseOS`, `Log Off kiliseOS`, `Welcome to kiliseOS`).
  - **Startup**: `winlogon.exe` (`"kiliseOS is starting up..."`).
  - **Installation & Setup**: `syssetup.dll` and `base/setup/reactos` (`"Welcome to the kiliseOS Setup Wizard"`, `"kiliseOS Workstation"`, `"kiliseOS Server"`).
  - **System Properties**: `sysdm.cpl` (clean kiliseOS banner, product strings, homepage link).
  - **Console & Utilities**: `consrv.dll` (`kiliseOS Console`), `calc`, `mspaint`, `mplay32`, `charmap`, `rapps`, etc.
  - **Display Properties**: `themeui.dll` and `desk.cpl` (`"kiliseOS Standard"`, `"kiliseOS Classic"`).

### 2.2 Strict Preservation of Open Source Credits & Licensing
- **NEVER** remove or obfuscate upstream Open Source attribution, contributor names, or legal licenses:
  - Upstream ReactOS, Wine, GNU GPL, and third-party developer credits in `IDS_DEVS` (`sysdm.rc`), `IDS_ACKTITLE` / `IDS_ACKSUBTITLE` (`syssetup.rc`), and `readme.txt` Section 2 must remain fully intact.
  - Header docstrings and copyright comments citing original authors in source files must be preserved verbatim.

---

## 3. Strict Technical & Binary Resource Constraints

### 3.1 Win32 ICO Container Rule (CRITICAL)
> [!CAUTION]
> **Windows NT 5.2 / ReactOS User32 DOES NOT support PNG-compressed icons.**
> Saving `.ico` files with PNG compression inside will cause immediate NULL pointer dereferences (`0x00000000 referenced memory could not be read`) and crash `explorer.exe` upon startup.

- **Mandatory Icon Generation Rule**:
  When generating `.ico` files using Python Pillow, **ALWAYS** enforce uncompressed Win32 DIB format by passing `bitmap_format="bmp"`:
  ```python
  img.save("target.ico", format="ICO", sizes=[(48, 48), (32, 32), (16, 16)], bitmap_format="bmp")
  ```
- Standard icon resolution suite: `48x48`, `32x32`, and `16x16` at 32-bit (RGBA) or 24-bit with 1-bit transparency mask.

### 3.2 Bitmap Specifications
- **LiveCD / Sysdm Banner**: `320x159` pixels, 24-bit RGB uncompressed BMP (`rosbitmap.bmp`) with companion 1-bit monochrome mask (`rosbitmap_mask.bmp`).
- **Shutdown / Logoff Flags**: `48x40` pixels, 24-bit RGB uncompressed BMP (`reactos_flag.bmp`) on dark container gradient (`#1D1D1D` to `#303030`).
- **Bootscreen Logo**: `ntoskrnl/inbv/resources/logo.bmp` (16-color 4-bit indexed DIB matching `inbv` palette format).

### 3.3 Ninja Incremental Build Cache Invalidation
> [!IMPORTANT]
> Ninja does not track deep resource dependencies inside `.rc` files. Modifying an icon or bitmap file on disk will NOT automatically trigger `windres` recompilation of `.rc.obj`.

- When adding, modifying, or replacing icons or bitmaps:
  Always delete or clear the corresponding object directories in `output-MinGW-i386/` (e.g. `output-MinGW-i386/dll/win32/shell32`, `output-MinGW-i386/dll/cpl`, `output-MinGW-i386/base/shell/explorer`, `output-MinGW-i386/dll/win32/msgina`) before invoking `ninja`.

---

## 4. Build, Verification & Testing Workflow

### 4.1 Build Environment
- Toolchain: RosBE MinGW i386 (`D:\Projects\kilsesi\.rosbe\i386\bin` and `D:\Projects\kilsesi\.rosbe\bin`).
- Build Directory: `D:\Projects\kilsesi\output-MinGW-i386`.
- Canonical build command:
  ```cmd
  cmd.exe /C "set PATH=D:\Projects\kilsesi\.rosbe\i386\bin;D:\Projects\kilsesi\.rosbe\bin;%PATH% && cd /D D:\Projects\kilsesi\output-MinGW-i386 && ninja livecd bootcd"
  ```

### 4.2 Local QEMU Testing
- Test LiveCD image in QEMU (via WSL or Windows native):
  ```bash
  qemu-system-i386 -accel tcg,thread=multi -m 1024M -smp 2 -cdrom /mnt/d/Projects/kilsesi/output-MinGW-i386/liveimg.iso -boot d -vga std -usb -device usb-tablet
  ```
- Verification Checklist:
  1. Boot screen shows custom kiliseOS logo without visual distortion.
  2. LiveCD initialization boots directly into desktop without `explorer.exe` crash.
  3. My Computer shows Papirus drive & system icons; LiveCD drive labeled `kiliseOS (X:)`.
  4. System Properties (`sysdm.cpl`) displays kiliseOS banner & "kiliseOS Operating System".
  5. Control Panel shows clean Papirus applet icons.
  6. Start Menu -> Turn Off dialog shows kiliseOS Globus Cruciger emblem and "Shut Down kiliseOS".

---

## 5. Git & Collaboration Hygiene

- Work on branch `master`.
- Commit messages follow Conventional Commits: `feat(...)`, `fix(...)`, `style(...)`, `refactor(...)`.
- Preserve the backup branch `macos-sonoma-backup` (stores historical experiments).
- Do not commit generated build artifacts (`output-MinGW-i386/`, `.iso` files) or temporary scratch scripts.
