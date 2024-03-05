<br/>
<div align="center">
    <br />
    <img src="https://github.com/romanpunia/asx/blob/main/var/assets/logo.png?raw=true" alt="ASX Logo" width="300" />
    <h3>AngelScript Runtime Environment</h3>
</div>

## About
ASX is a memory efficient AngelScript environment implementation similar to Node.js.

## Usage
Simplest examples:
```bash
# Show all commands:
  asx -h
  asx --help
# Execute a script file:
  asx [path] [args]
# Debug a script file:
  asx -d [path] [args]
# Install dependencies of a script file
  asx -i [path]
```
Also note: -option and --option both have different syntax:
```bash
# Shorthand style
  asx -option [value?]
# Normal style
  asx --option=[value?]
```

## Preprocessor
Scripts support preprocessor that can be used just like C++ preprocessor for dependency management. Preprocessed script lines of code could get shuffled around, however if compile or runtime error happens you will get last N lines of source code where it happend (including column pointer).
```cpp

// Global include search
#include <console.as> // Standard library include
#include <console> // Shorter version
#include <addon.so> // Linux SO (global search will be used)
#include <addon.dylib> // Mac SO
#include <addon.dll> // Windows SO
#include <addon> // Or automatic search

// Local and remote include search
#include "file" // Short include version
#include "file.as" // Will include file at current directory
#include "./file.as" // Same but more verbose
#include "./../file.as" // Will include file from parent directory
#include "../file.as" // Same but less verbose
#include "https://raw.githubusercontent.com/romanpunia/asx/main/bin/examples/processes.as"

// Remote addon includes
// This will clone a git repository and build it if it is a native addon as shared library
// If repository is vm based (meaning no native code) then index file will be used
// Directory that includes this remote addon will get subdirectory <addons> with simple caching system
// Addon must contain <addon.json> configuration file (which is simple and can be generated by ASX automatically: --addon)
#include "@romanpunia/test.as"
```

Modern imports based on AngelScript function imports and TypeScript imports (mostly similar to C++ style includes)
```py
import from "console";
import from "./file";
import from {
    "console",
    "./file"
};
```

Scripts are written using AngelScript syntax, standard library defers from addons at angelcode. Preprocessor is similar to one that C has but simpler, simple preprocessor macro statements are supported. Entrypoint is defined by these function signatures:
* void main()
* int main()
* int main(string[]@)

```cpp
#include <string> // By default string class is not exposed

void main() { }
int main() { return 0; }
int main(string[]@ args)
{
#define SUM(a, b) ((a) + (b))
#ifdef SUM
    return SUM(1, 2);
#else
    return 0;
#endif
}
```

There are also modifiers for main function:
```cs
/* Shows console automatically (if not shown) */
#[console::main]
void main() { }

/* Starts task scheduler (for async io) */
#[schedule::main]
void main() { }

/*
  Starts task scheduler with parameters:
    "threads" - threads to spawn (default: auto)
    "stop" - stop scheduler after leaving main (default: false)
*/
#[schedule::main(threads = 8, stop = true)]
void main() { }
```

Preprocessor also supports shared object imports. They are not considered addons or plugins in any way. They can be used to implement some low level functionality without accessing C++ code. More on that in **bin/examples/processes.as**.
```cpp

#pragma cimport("kernel32.dll", "GetCurrentProcessId", "uint32 win32_get_pid()") // SO filename or path, function name to find in SO, function definition to expose to AngelScript.

int main()
{
#ifdef SOF_GetCurrentProcessId // SOF = shared object function, SOF_* will be defined if function has successfully been exposed
    return win32_get_pid();
#else
    return 0;
#endif
}
```

## Addons (local and remote)
There is support for addons. Addons must be compiled with Vitex as a shared object dependency or they must load Vitex symbols manually to work properly. To initialize a VM addon use following command:
```bash
# Will create a directory named "example" in current working directory, directory will contain AngelScript files
  asx --target=example --addon=vm:.
```

Addon can also be a C++ shared library that implements following methods:
```cpp
#include <vitex/scripting.h>

extern "C" { VI_EXPOSE int ViInitialize(Vitex::Scripting::VirtualMachine*); }
int ViInitialize(Vitex::Scripting::VirtualMachine* VM) // Required initialization for requested virtual machine
{
    return 0; // Zero is successful initialization
}

extern "C" { VI_EXPOSE void ViUninitialize(Vitex::Scripting::VirtualMachine*); }
void ViUninitialize(Vitex::Scripting::VirtualMachine* VM) // Optional deinitialization for requested virtual machine
{
}
```
You can create your own addon using _--addon_ command. This will create either a new native addon or vm addon template in specified directory, don't forget to name it using _--target_ command.

After that you will either have a ready to use git repository with CMake configuration for C++ project with example code above. Or you will get two files _addon.json_ and _addon.as_. Keep in mind that without having _addon.json_ repository won't be evaluated as addon. Generated repository can be pushed to github and used afterwards with:
```ts
/* Points to: https://github.com/repo_owner/repo_name */
import from "@repo_owner/repo_name"; // Must start with <@> symbol
```

Keep in mind that if you use remote addons which is a feature that works similiar to NPM, you will get \<addons\> directory near you executable script that is basically \<node_modules\> directory. First time builds (if native) are slow as it is required to download full Vitex framework beforehand and build the target addon using platform compiler, after shared library has been built loading times are submillisecond.

To install dependencies of a program run _--install_ and provide a file that should be scanned for dependencies. This will only download and build required addons. Afterwards you may run a program.

* Native addons are made using C++. Compilation is greedy.
* VM addons are made using AngelScript. Compilation is on demand.

## Debugging
You may just run asx with _--debug_ or _-d_ flag. This will allocate resources for debugger context and before executing anything it will debug-stop. Type _help_ to view available commands:
```bash
# Will execute input file with debugging interface attached and game engine mode enabled
  asx -d -g examples/rendering
```

## Binary generation and packaging
ASX supports a feature that allows one to build the executable from AngelScript program. To build an executable use following command:
```bash
# Build an example program (cwd is asx/bin/examples), you may run it multiple times (minimal rebuilds are enabled)
# Will create a directory named "quad" near "rendering.as" file, directory will contain CMake project named "quad" and built targets
  asx --install --target=quad --output=. -g examples/rendering.as
```

This will produce a binary and shared libraries. Amount of shared libraries produced will depend on import statements inside your script. For example, you won't be needing an OpenAL shared library if you don't use **audio**.

AngelScript VM will be configured according to your ASX setup. Your AngelScript source code will be compiled to platform-independant bytecode. This bytecode will then be hex-encoded and embedded into your binary as executable text.

Generated output will not embed any resources requested by runtime such as images, files, audio and other resources. You will have to add (and optionally pack) them manually as in usual C++ project.

## Performance
Currently, the main issue is initialization time. About 40ms (app-mode) or 210ms (game-mode) of time is taken by initialization that does not include script source code compilation or execution. However it does not mean this time will grow as dramatically as Node.js initialization time when loading many CommonJS modules.

The framework is set up in a pessimistic mode which leaves assertion statements in release mode to ensure valid panic state in case of misuse of APIs. This introduces performance penalty (in some cases severe) in return for program correctness.

You may also check performance benchmarks in **bin/examples/stresstest\*.as**. First is singlethreaded mode, second is multithreaded mode. You may run these scripts with a single argument that will be a number higher than zero (usually pretty big number). This example will calculate some 64-bit integer hash based on input.

## Memory usage
Generally, AngelScript uses much less memory than v8 JavaScript runtime. That is because there are practically no wrappers between C++ types and AngelScript types.

## Other info
You may take a look into __html.as__ example which leverages HTML/CSS + AngelScript powers. This shows how to create memory and CPU efficient GUI applications based on modern graphics API. 

## Dependencies
* [Vitex (submodule)](https://github.com/romanpunia/vitex)

## Building
There are several ways to build this project that are explained here (similar to Vitex):
* [Manually performed builds](var/MANUAL.md)
* [Precomposed docker builds](var/DOCKER.md)

## License
ASX is licensed under the MIT license