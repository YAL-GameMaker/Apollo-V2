## Preparing to build

Needless to say, this requires basic familiarity with Visual Studio, Command Prompt/PowerShell, and Windows in general.

**Note:** GmxGen is already set up in the VS solution
so these steps are enough - you do not have to follow build-setup instructions
from GmxGen's README.

### Setting up Lua
Download a [Lua release](https://lua.org/download.html) and extract it to the Lua directory so that `lua.h` is at `Lua/lua.h`

### Setting up GmxGen

1. [Install Haxe](https://haxe.org/download/) (make sure to install Neko VM!)
2. [Download the source code](https://github.com/YAL-GameMaker-Tools/GmxGen/archive/refs/heads/master.zip) 
(or [check out the git repository](https://github.com/YAL-GameMaker-Tools/GmxGen))
3. Compile the program: `haxe build-neko.hxml`
4. Create an executable: `nekotools boot bin/GmxGen.n`
5. Copy `bin/GmxGen.exe` to a folder in your PATH (e.g. to the Haxe directory )

---

**Note:** if you've had Visual Studio open while doing this, you'll want to re-open it -
changes to PATH do not take effect for existing windows.

## Building

Open the `.sln` in Visual Studio (VS2019 or VS2022 are both fine - you can change build tools),
and for x64 - Release.

If you have correctly set up `GmxGen` and `GmlCppExtFuncs`,
the project will generate the `autogen.gml` files for GML<->C++ interop during pre-build
and will copy and \[re-]link files during post-build.

For Mac/Linux, run `build-mac.sh` or `build-linux.sh` respectively