![GDAlert logo](/logo.png)

## GDAlert

This is an effort to create a rich and portable frontend for the [open source release](https://github.com/electronicarts/CnC_Remastered_Collection) of the [Command & Conquer Remastered Collection](https://store.steampowered.com/app/1213210/Command__Conquer_Remastered_Collection/) .DLL files in the free and open source [Godot engine](https://godotengine.org/).

### Goals
* Implement modern UX features like high resolutions and smooth scrolling.
* Support the original games' assets in their original form.
* Maintain strict compatibility with the retail libraries so that mods for the official release can be played and tested using GDAlert.
* Facilitate porting Red Alert and Tiberian Dawn to platforms without a mouse and keyboard, like mobile or consoles.

### Long-term maybes
* 3D-rendered graphics
* VR support?

### Building
Unfortunately, due to the poor portability of the original games' code, this project currently only supports 32-bit Windows as a platform. Progress is being made elsewhere in the community to remedy this, but for now, Windows 10 and MSVC are requirements.

#### Requirements
* [git](https://git-scm.com/) (Available in your PATH)
* [Python](https://www.python.org/), latest 2.x or 3.x (Available in your PATH)
  * [scons](https://scons.org/): `pip install scons`
* [CMake](https://cmake.org/), 3.10 or higher (Available in your PATH)
* [Visual Studio 2019](https://visualstudio.microsoft.com/vs/)
  * C++ Build Tools v142 (Older might work)
  * Windows 10 SDK (Windows 8 SDK might work)
* [Godot 3.2.1 for Win32](https://godotengine.org/download/windows) (NOT Win64)

#### Steps
There is currently no CMake support in GDAlert itself, so for now there's just a simple batch file that runs the commands to build the project's dependencies.

Remember that you need to have `git`, `python` (with `scons`), and `cmake` all available in your `PATH` or otherwise runnable within the Native Tools Command Prompt.

**You must run the following command in the `x86 Native Tools Command Prompt for VS 2019`, which is included with Visual Studio 2019:**

`build-deps.bat`

Once this script is finished, you can open `GDAlert.sln` in Visual Studio and build. Then, you can open the `GDAlert` folder in the Godot editor.
