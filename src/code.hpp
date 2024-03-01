#ifndef HAS_CODE_BUNDLE
#define HAS_CODE_BUNDLE
#include <string>

namespace code_bundle
{
	void foreach(void* context, void(*callback)(void*, const char*, const char*, unsigned))
	{
		if (!callback)
			return;

		const char* sc_addon_cmakelists_txt = "cmake_minimum_required(VERSION 3.6)\nset(VI_DIRECTORY \"{{BUILDER_VITEX_PATH}}\" CACHE STRING \"Vitex directory\")\n{{BUILDER_FEATURES}}\ninclude(${VI_DIRECTORY}/lib/toolchain.cmake)\nproject({{BUILDER_OUTPUT}})\nif (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)\n    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)\n    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)\n    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)\n    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin)\n    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)\n    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)\n    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)\n    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin)\n    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)\n    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)\n    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)\n    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin)\nendif()\nadd_subdirectory(${VI_DIRECTORY} vitex)\nlink_directories(${VI_DIRECTORY})\nadd_library({{BUILDER_OUTPUT}} SHARED\n    ${CMAKE_CURRENT_SOURCE_DIR}/src/addon.cpp)\nset_target_properties({{BUILDER_OUTPUT}} PROPERTIES\n    OUTPUT_NAME \"{{BUILDER_OUTPUT}}\"\n    CXX_STANDARD ${VI_CXX}\n    CXX_STANDARD_REQUIRED ON\n    CXX_EXTENSIONS OFF)\ntarget_include_directories({{BUILDER_OUTPUT}} PRIVATE ${VI_DIRECTORY})\ntarget_link_libraries({{BUILDER_OUTPUT}} PRIVATE vitex)";
		callback(context, "addon/CMakeLists.txt", sc_addon_cmakelists_txt, 1569);

		const char* sc_addon_addon_as = "import from \"console\";\n\nvoid print_hello_world()\n{\n    console@ log = console::get();\n    log.write_line(\"Hello, World\");\n}";
		callback(context, "addon/addon.as", sc_addon_addon_as, 123);

		const char* sc_addon_addon_cpp = "#include <vitex/scripting.h>\n#include <iostream>\n\nvoid PrintHelloWorld()\n{\n    std::cout << \"Hello, World!\" << std::endl;\n}\n\nextern \"C\" { VI_EXPOSE int ViInitialize(Vitex::Scripting::VirtualMachine*); }\nint ViInitialize(Vitex::Scripting::VirtualMachine* VM)\n{\n    VM->SetFunction(\"void print_hello_world()\", &PrintHelloWorld);\n    return 0;\n}\n\nextern \"C\" { VI_EXPOSE void ViUninitialize(Vitex::Scripting::VirtualMachine*); }\nvoid ViUninitialize(Vitex::Scripting::VirtualMachine* VM)\n{\n}";
		callback(context, "addon/addon.cpp", sc_addon_addon_cpp, 486);

		const char* sc_addon_addon_json = "{\n    \"name\": \"{{BUILDER_OUTPUT}}\",\n    \"type\": \"{{BUILDER_MODE}}\",\n    \"runtime\": \"{{BUILDER_VERSION}}\",\n    \"version\": \"1.0.0\",\n    \"index\": {{BUILDER_INDEX}}\n}";
		callback(context, "addon/addon.json", sc_addon_addon_json, 162);

		const char* sc_executable_cmakelists_txt = "cmake_minimum_required(VERSION 3.6)\nset(VI_DIRECTORY \"{{BUILDER_VITEX_PATH}}\" CACHE STRING \"-\")\n{{BUILDER_FEATURES}}\ninclude(${VI_DIRECTORY}/deps/toolchain.cmake)\nproject({{BUILDER_OUTPUT}})\nset(CMAKE_DISABLE_IN_SOURCE_BUILD ON)\nset(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)\nset(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)\nset(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)\nset(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin)\nset(BUFFER_DATA \"#ifndef HAS_PROGRAM_BYTECODE\\n#define HAS_PROGRAM_BYTECODE\\n#include <string>\\n\\nnamespace program_bytecode\\n{\\n\\tvoid foreach(void* context, void(*callback)(void*, const char*, unsigned))\\n\\t{\\n\\t\\tif (!callback)\\n\\t\\t\\treturn;\\n\")\nset(BUFFER_OUT \"${CMAKE_SOURCE_DIR}/program\")\nset(FILENAME \"program_bytecode\")\nfile(READ \"${CMAKE_SOURCE_DIR}/program.b64\" FILEDATA)\nif (NOT FILEDATA STREQUAL \"\")\n    string(LENGTH \"${FILEDATA}\" FILESIZE)\n    if (FILESIZE GREATER 4096)\n        set(FILEOFFSET 0)\n        string(APPEND BUFFER_DATA \"\\n\\t\\tstd::string dc_${FILENAME};\\n\\t\\tdc_${FILENAME}.reserve(${FILESIZE});\")\n        while (FILEOFFSET LESS FILESIZE)\n            math(EXPR CHUNKSIZE \"${FILESIZE}-${FILEOFFSET}\")\n            if (CHUNKSIZE GREATER 4096)\n                set(CHUNKSIZE 4096)\n                string(SUBSTRING \"${FILEDATA}\" \"${FILEOFFSET}\" \"${CHUNKSIZE}\" CHUNKDATA)\n            else()\n                string(SUBSTRING \"${FILEDATA}\" \"${FILEOFFSET}\" \"-1\" CHUNKDATA)\n            endif()\n            string(APPEND BUFFER_DATA \"\\n\\t\\tdc_${FILENAME} += \\\"${CHUNKDATA}\\\";\")\n            math(EXPR FILEOFFSET \"${FILEOFFSET}+${CHUNKSIZE}\")\n        endwhile()\n        string(APPEND BUFFER_DATA \"\\n\\t\\tcallback(context, dc_${FILENAME}.c_str(), (unsigned int)dc_${FILENAME}.size());\\n\")\n    else()\n        string(APPEND BUFFER_DATA \"\\n\\t\\tconst char* sc_${FILENAME} = \\\"${FILEDATA}\\\";\\n\\t\\tcallback(context, sc_${FILENAME}, ${FILESIZE});\\n\")\n    endif()    \nendif()\nstring(APPEND BUFFER_DATA \"\\t}\\n}\\n#endif\")\nfile(WRITE ${BUFFER_OUT}.hpp \"${BUFFER_DATA}\")\t\nlist(APPEND SOURCE \"${BUFFER_OUT}.hpp\")\nadd_executable({{BUILDER_OUTPUT}}\n    ${CMAKE_CURRENT_SOURCE_DIR}/runtime.hpp\n    ${CMAKE_CURRENT_SOURCE_DIR}/program.hpp\n    ${CMAKE_CURRENT_SOURCE_DIR}/program.cpp)\nset_target_properties({{BUILDER_OUTPUT}} PROPERTIES\n    OUTPUT_NAME \"{{BUILDER_OUTPUT}}\"\n    CXX_STANDARD ${VI_CXX}\n    CXX_STANDARD_REQUIRED ON\n    CXX_EXTENSIONS OFF\n    VERSION ${PROJECT_VERSION}\n    SOVERSION ${PROJECT_VERSION})\nif (MSVC AND {{BUILDER_APPLICATION}})\n    set(CMAKE_EXE_LINKER_FLAGS \"/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS\")\nendif()\nadd_subdirectory(${VI_DIRECTORY} vitex)\nlink_directories(${VI_DIRECTORY})\ntarget_include_directories({{BUILDER_OUTPUT}} PRIVATE ${VI_DIRECTORY})\ntarget_link_libraries({{BUILDER_OUTPUT}} PRIVATE vitex)";
		callback(context, "executable/CMakeLists.txt", sc_executable_cmakelists_txt, 2830);

		std::string dc_executable_program_cpp;
		dc_executable_program_cpp.reserve(5077);
		dc_executable_program_cpp += "#include \"program.hpp\"\n#include \"runtime.hpp\"\n#include <vitex/vitex.h>\n#include <vitex/bindings.h>\n#include <vitex/engine.h>\n#include <signal.h>\n\nusing namespace Vitex::Engine;\nusing namespace ASX;\n\nEventLoop* Loop = nullptr;\nVirtualMachine* VM = nullptr;\nCompiler* Unit = nullptr;\nImmediateContext* Context = nullptr;\nstd::mutex Mutex;\nint ExitCode = 0;\n\nvoid exit_program(int sigv)\n{\n\tif (sigv != SIGINT && sigv != SIGTERM)\n        return;\n\n\tUMutex<std::mutex> Unique(Mutex);\n    {\n        if (Runtime::TryContextExit(EnvironmentConfig::Get(), sigv))\n        {\n\t\t\tLoop->Wakeup();\n            goto GracefulShutdown;\n        }\n\n        auto* App = Application::Get();\n        if (App != nullptr && App->GetState() == ApplicationState::Active)\n        {\n            App->Stop();\n\t\t\tLoop->Wakeup();\n            goto GracefulShutdown;\n        }\n\n        if (Schedule::IsAvailable())\n        {\n            Schedule::Get()->Stop();\n\t\t\tLoop->Wakeup();\n            goto GracefulShutdown;\n        }\n\n        return std::exit((int)ExitStatus::Kill);\n    }\nGracefulShutdown:\n    signal(sigv, &exit_program);\n}\nvoid setup_program(EnvironmentConfig& Env)\n{\n    OS::Directory::SetWorking(Env.Path.c_str());\n    signal(SIGINT, &exit_program);\n    signal(SIGTERM, &exit_program);\n#ifdef VI_UNIX\n    signal(SIGPIPE, SIG_IGN);\n    signal(SIGCHLD, SIG_IGN);\n#endif\n}\nbool load_program(EnvironmentConfig& Env)\n{\n#ifdef HAS_PROGRAM_BYTECODE\n    program_bytecode::foreach(&Env, [](void* Context, const char* Buffer, unsigned Size)\n    {\n        EnvironmentConfig* Env = (EnvironmentConfig*)Context;\n\t    Env->Program = Codec::Base64Decode(std::string_view(Buffer, (size_t)Size));\n    });\n    return true;\n#else\n    return false;\n#endif\n}\nint main(int argc, char* argv[])\n{\n\tEnvironmentConfig Env;\n\tEnv.Path = *OS::Directory::GetModule();\n\tEnv.Module = argc > 0 ? argv[0] : \"runtime\";\n\tEnv.AutoSchedule = {{BUILDER_ENV_AUTO_SCHEDULE}};\n\tEnv.AutoConsole = {{BUILDER_ENV_AUTO_CONSOLE}};\n\tEnv.AutoStop = {{BUILDER_ENV_AUTO_STOP}};\n    if (!load_program(Env))\n        return 0;\n\n\tVector<String> Args;\n\tArgs.reserve((size_t)argc);\n\tfor (int i = 0; i < argc; i++)\n\t\tArgs.push_back(argv[i]);\n\n\tSystemConfig Config;\n\tConfig.Permissions = { {{BUILDER_CONFIG_PERMISSIONS}} };\n\tConfig.Libraries = { {{BUILDER_CONFIG_LIBRARIES}} };\n\tConfig.Functions = { {{BUILDER_CONFIG_FUNCTIONS}} };\n\tConfig.SystemAddons = { {{BUILDER_CONFIG_ADDONS}} };\n\tConfig.Tags = {{BUILDER_CONFIG_TAGS}};\n\tConfig.TsImports = {{BUILDER_CONFIG_TS_IMPORTS}};\n\tConfig.EssentialsOnly = {{BUILDER_CONFIG_ESSENTIALS_ONLY}};\n    setup_program(Env);\n\n\tVitex::Runtime Scope(Config.EssentialsOnly ? (size_t)Vitex::Preset::App : (size_t)Vitex::Preset::Game);\n\t{\n\t\tVM = new VirtualMachine();\n\t\tUnit = VM->CreateCompiler();\n        Context = VM->RequestContext();\n\t\t\n        Vector<std::pair<uint32_t, size_t>> Settings = { {{BUILDER_CONFIG_SETTINGS}} };\n        for (auto& Item : Settings)\n            VM->SetProperty((Features)Item.first, Item.second);\n\n\t\tUnit = VM->CreateCompiler();\n\t\tExitCode = Runtime::ConfigureContext(Config, Env, VM, Unit) ? (int)ExitStatus::OK : (int)ExitStatus::CompilerError;\n\t\tif (ExitCode != (int)ExitStatus::OK)\n\t\t\tgoto FinishProgram;\n\n\t\tRuntime::ConfigureSystem(Config);\n\t\tif (!Unit->Prepare(Env.Module))\n\t\t{\n\t\t\tVI_ERR(\"cannot prepare <%s> module scope\", Env.Module);\n\t\t\tExitCode = (int)ExitStatus::PrepareError;\n\t\t\tgoto FinishProgram;\n\t\t}\n\n\t\tByteCodeInfo Info;\n\t\tInfo.Data.insert(Info.Data.begin(), Env.Program.begin(), Env.Program.end());\n\t\tif (!Unit->LoadByteCode(&Info).Get())\n\t\t{\n\t\t\tVI_ERR(\"cannot load <%s> module bytecode\", Env.Module);\n\t\t\tExitCode = (int)ExitStatus::LoadingError;\n\t\t\tgoto FinishProgram;\n\t\t}\n\n\t    ProgramEntrypoint Entrypoint;\n\t\tFunction Main = Runtime::GetEntrypoint(Env, Entrypoint, Unit);\n\t\tif (!Main.IsValid())\n        {\n\t\t\tExitCode = (int)ExitStatus::EntrypointError;\n\t\t\tgoto FinishProgram;\n        }\n\n\t\tint ExitCode = 0;\n\t\tTypeInfo Type = VM->GetTypeInfoByDecl(\"array<string>@\");\n\t\tBindings::Array* ArgsArray = Type.IsValid() ? Bindings::Array::Compose<String>(Type.GetTypeInfo(), Args) : nullptr;\n\t\tVM->S";
		dc_executable_program_cpp += "etExceptionCallback([](ImmediateContext* Context)\n\t\t{\n\t\t\tif (!Context->WillExceptionBeCaught())\n\t\t\t\tstd::exit((int)ExitStatus::RuntimeError);\n\t\t});\n\n\t\tMain.AddRef();\n\t\tLoop = new EventLoop();\n\t\tLoop->Listen(Context);\n\t\tLoop->Enqueue(FunctionDelegate(Main, Context), [&Main, ArgsArray](ImmediateContext* Context)\n\t\t{\n\t\t\tRuntime::StartupEnvironment(EnvironmentConfig::Get());\n\t\t\tif (Main.GetArgsCount() > 0)\n\t\t\t\tContext->SetArgObject(0, ArgsArray);\n\t\t}, [&ExitCode, &Type, &Main, ArgsArray](ImmediateContext* Context)\n\t\t{\n\t\t\tExitCode = Main.GetReturnTypeId() == (int)TypeId::VOIDF ? 0 : (int)Context->GetReturnDWord();\n\t\t\tif (ArgsArray != nullptr)\n\t\t\t\tContext->GetVM()->ReleaseObject(ArgsArray, Type);\n\t\t\tRuntime::ShutdownEnvironment(EnvironmentConfig::Get());\n\t\t\tLoop->Wakeup();\n\t\t});\n        \n\t\tRuntime::AwaitContext(Mutex, Loop, VM, Context);\n\t}\nFinishProgram:\n\tMemory::Release(Context);\n\tMemory::Release(Unit);\n\tMemory::Release(VM);\n    Memory::Release(Loop);\n\treturn ExitCode;\n}";
		callback(context, "executable/program.cpp", dc_executable_program_cpp.c_str(), (unsigned int)dc_executable_program_cpp.size());

		std::string dc_executable_runtime_hpp;
		dc_executable_runtime_hpp.reserve(7319);
		dc_executable_runtime_hpp += "#ifndef RUNTIME_H\n#define RUNTIME_H\n#include <vitex/bindings.h>\n#include <vitex/vitex.h>\n\nusing namespace Vitex::Core;\nusing namespace Vitex::Compute;\nusing namespace Vitex::Scripting;\n\nnamespace ASX\n{\n\tenum class ExitStatus\n\t{\n\t\tContinue = 0x00fffff - 1,\n\t\tOK = 0,\n\t\tRuntimeError,\n\t\tPrepareError,\n\t\tLoadingError,\n\t\tSavingError,\n\t\tCompilerError,\n\t\tEntrypointError,\n\t\tInputError,\n\t\tInvalidCommand,\n\t\tInvalidDeclaration,\n\t\tCommandError,\n\t\tKill\n\t};\n\n\tstruct ProgramEntrypoint\n\t{\n\t\tconst char* ReturnsWithArgs = \"int main(array<string>@)\";\n\t\tconst char* Returns = \"int main()\";\n\t\tconst char* Simple = \"void main()\";\n\t};\n\n\tstruct EnvironmentConfig\n\t{\n\t\tInlineArgs Commandline;\n\t\tUnorderedSet<String> Addons;\n\t\tFunctionDelegate AtExit;\n\t\tFileEntry File;\n\t\tString Name;\n\t\tString Path;\n\t\tString Program;\n\t\tString Registry;\n\t\tString Mode;\n\t\tString Output;\n\t\tString Addon;\n\t\tCompiler* ThisCompiler;\n\t\tconst char* Module;\n\t\tint32_t AutoSchedule;\n\t\tbool AutoConsole;\n\t\tbool AutoStop;\n\t\tbool Inline;\n\n\t\tEnvironmentConfig() : ThisCompiler(nullptr), Module(\"__anonymous__\"), AutoSchedule(-1), AutoConsole(false), AutoStop(false), Inline(true)\n\t\t{\n\t\t}\n\t\tvoid Parse(int ArgsCount, char** ArgsData, const UnorderedSet<String>& Flags = { })\n\t\t{\n\t\t\tCommandline = OS::Process::ParseArgs(ArgsCount, ArgsData, (size_t)ArgsFormat::KeyValue | (size_t)ArgsFormat::FlagValue | (size_t)ArgsFormat::StopIfNoMatch, Flags);\n\t\t}\n\t\tstatic EnvironmentConfig& Get(EnvironmentConfig* Other = nullptr)\n\t\t{\n\t\t\tstatic EnvironmentConfig* Base = Other;\n\t\t\tVI_ASSERT(Base != nullptr, \"env was not set\");\n\t\t\treturn *Base;\n\t\t}\n\t};\n\n\tstruct SystemConfig\n\t{\n\t\tUnorderedMap<String, std::pair<String, String>> Functions;\n\t\tUnorderedMap<AccessOption, bool> Permissions;\n\t\tVector<std::pair<String, bool>> Libraries;\n\t\tVector<std::pair<String, int32_t>> Settings;\n\t\tVector<String> SystemAddons;\n\t\tbool TsImports = true;\n\t\tbool Tags = true;\n\t\tbool Debug = false;\n\t\tbool Interactive = false;\n\t\tbool EssentialsOnly = true;\n\t\tbool PrettyProgress = true;\n\t\tbool LoadByteCode = false;\n\t\tbool SaveByteCode = false;\n\t\tbool SaveSourceCode = false;\n\t\tbool Dependencies = false;\n\t\tbool Install = false;\n\t\tsize_t Installed = 0;\n\t};\n\n\tclass Runtime\n\t{\n\tpublic:\n\t\tstatic void StartupEnvironment(EnvironmentConfig& Env)\n\t\t{\n\t\t\tif (Env.AutoSchedule >= 0)\n\t\t\t\tSchedule::Get()->Start(Env.AutoSchedule > 0 ? Schedule::Desc((size_t)Env.AutoSchedule) : Schedule::Desc());\n\n\t\t\tif (Env.AutoConsole)\n\t\t\t\tConsole::Get()->Attach();\n\t\t}\n\t\tstatic void ShutdownEnvironment(EnvironmentConfig& Env)\n\t\t{\n\t\t\tif (Env.AutoStop)\n\t\t\t\tSchedule::Get()->Stop();\n\t\t}\n\t\tstatic void ConfigureSystem(SystemConfig& Config)\n\t\t{\n\t\t\tfor (auto& Option : Config.Permissions)\n\t\t\t\tOS::Control::Set(Option.first, Option.second);\n\t\t}\n\t\tstatic bool ConfigureContext(SystemConfig& Config, EnvironmentConfig& Env, VirtualMachine* VM, Compiler* ThisCompiler)\n\t\t{\n\t\t\tVM->SetTsImports(Config.TsImports);\n\t\t\tVM->SetModuleDirectory(OS::Path::GetDirectory(Env.Path.c_str()));\n\t\t\tVM->SetPreserveSourceCode(Config.SaveSourceCode);\n\n\t\t\tfor (auto& Name : Config.SystemAddons)\n\t\t\t{\n\t\t\t\tif (!VM->ImportSystemAddon(Name))\n\t\t\t\t{\n\t\t\t\t\tVI_ERR(\"system addon <%s> cannot be loaded\", Name.c_str());\n\t\t\t\t\treturn false;\n\t\t\t\t}\n\t\t\t}\n\n\t\t\tfor (auto& Path : Config.Libraries)\n\t\t\t{\n\t\t\t\tif (!VM->ImportCLibrary(Path.first, Path.second))\n\t\t\t\t{\n\t\t\t\t\tVI_ERR(\"external %s <%s> cannot be loaded\", Path.second ? \"addon\" : \"clibrary\", Path.first.c_str());\n\t\t\t\t\treturn false;\n\t\t\t\t}\n\t\t\t}\n\n\t\t\tfor (auto& Data : Config.Functions)\n\t\t\t{\n\t\t\t\tif (!VM->ImportCFunction({ Data.first }, Data.second.first, Data.second.second))\n\t\t\t\t{\n\t\t\t\t\tVI_ERR(\"clibrary function <%s> from <%s> cannot be loaded\", Data.second.first.c_str(), Data.first.c_str());\n\t\t\t\t\treturn false;\n\t\t\t\t}\n\t\t\t}\n\n\t\t\tauto* Macro = ThisCompiler->GetProcessor();\n\t\t\tMacro->AddDefaultDefinitions();\n\n\t\t\tEnv.ThisCompiler = ThisCompiler;\n\t\t\tBindings::Tags::BindSyntax(VM, Config.Tags, &Runtime::ProcessTags);\n\t\t\tEnvironmentConfig::Get(&Env);\n\n\t\t\tVM->ImportSystemAddon(\"ctypes\");\n\t\t\tVM->BeginNamespace(\"this_process\");\n\t\t\tVM->SetFunctionDef(\"void exit_event(int)\");\n\t\t\tVM->SetFunction(\"void be";
		dc_executable_runtime_hpp += "fore_exit(exit_event@)\", &Runtime::ApplyContextExit);\n\t\t\tVM->SetFunction(\"uptr@ get_compiler()\", &Runtime::GetCompiler);\n\t\t\tVM->EndNamespace();\n\t\t\treturn true;\n\t\t}\n\t\tstatic bool TryContextExit(EnvironmentConfig& Env, int Value)\n\t\t{\n\t\t\tif (!Env.AtExit.IsValid())\n\t\t\t\treturn false;\n\n\t\t\tauto Status = Env.AtExit([Value](ImmediateContext* Context)\n\t\t\t{\n\t\t\t\tContext->SetArg32(0, Value);\n\t\t\t}).Get();\n\t\t\tEnv.AtExit.Release();\n\t\t\tVirtualMachine::CleanupThisThread();\n\t\t\treturn !!Status;\n\t\t}\n\t\tstatic void ApplyContextExit(asIScriptFunction* Callback)\n\t\t{\n\t\t\tauto& Env = EnvironmentConfig::Get();\n\t\t\tUPtr<ImmediateContext> Context = Callback ? Env.ThisCompiler->GetVM()->RequestContext() : nullptr;\n\t\t\tEnv.AtExit = FunctionDelegate(Callback, *Context);\n\t\t}\n\t\tstatic void AwaitContext(std::mutex& Mutex, EventLoop* Loop, VirtualMachine* VM, ImmediateContext* Context)\n\t\t{\n\t\t\tEventLoop::Set(Loop);\n\t\t\twhile (Loop->PollExtended(Context, 1000))\n\t\t\t{\n\t\t\t\tVM->PerformPeriodicGarbageCollection(60000);\n\t\t\t\tLoop->Dequeue(VM);\n\t\t\t}\n\n\t\t\tUMutex<std::mutex> Unique(Mutex);\n\t\t\tif (Schedule::HasInstance())\n\t\t\t{\n\t\t\t\tauto* Queue = Schedule::Get();\n\t\t\t\twhile (!Queue->CanEnqueue() && Queue->HasAnyTasks())\n\t\t\t\t\tQueue->Dispatch();\n\t\t\t}\n\n\t\t\tEventLoop::Set(nullptr);\n\t\t\tContext->Reset();\n\t\t\tVM->PerformFullGarbageCollection();\n\t\t\tApplyContextExit(nullptr);\n\t\t}\n\t\tstatic Function GetEntrypoint(EnvironmentConfig& Env, ProgramEntrypoint& Entrypoint, Compiler* Unit, bool Silent = false)\n\t\t{\n\t\t\tFunction MainReturnsWithArgs = Unit->GetModule().GetFunctionByDecl(Entrypoint.ReturnsWithArgs);\n\t\t\tFunction MainReturns = Unit->GetModule().GetFunctionByDecl(Entrypoint.Returns);\n\t\t\tFunction MainSimple = Unit->GetModule().GetFunctionByDecl(Entrypoint.Simple);\n\t\t\tif (MainReturnsWithArgs.IsValid() || MainReturns.IsValid() || MainSimple.IsValid())\n\t\t\t\treturn MainReturnsWithArgs.IsValid() ? MainReturnsWithArgs : (MainReturns.IsValid() ? MainReturns : MainSimple);\n\n\t\t\tif (!Silent)\n\t\t\t\tVI_ERR(\"module %s must contain either: <%s>, <%s> or <%s>\", Env.Module, Entrypoint.ReturnsWithArgs, Entrypoint.Returns, Entrypoint.Simple);\n\t\t\treturn Function(nullptr);\n\t\t}\n\t\tstatic Compiler* GetCompiler()\n\t\t{\n\t\t\treturn EnvironmentConfig::Get().ThisCompiler;\n\t\t}\n\n\tprivate:\n\t\tstatic void ProcessTags(VirtualMachine* VM, Bindings::Tags::TagInfo&& Info)\n\t\t{\n\t\t\tauto& Env = EnvironmentConfig::Get();\n\t\t\tfor (auto& Tag : Info)\n\t\t\t{\n\t\t\t\tif (Tag.Name != \"main\")\n\t\t\t\t\tcontinue;\n\n\t\t\t\tfor (auto& Directive : Tag.Directives)\n\t\t\t\t{\n\t\t\t\t\tif (Directive.Name == \"#schedule::main\")\n\t\t\t\t\t{\n\t\t\t\t\t\tauto Threads = Directive.Args.find(\"threads\");\n\t\t\t\t\t\tif (Threads != Directive.Args.end())\n\t\t\t\t\t\t\tEnv.AutoSchedule = FromString<uint8_t>(Threads->second).Or(0);\n\t\t\t\t\t\telse\n\t\t\t\t\t\t\tEnv.AutoSchedule = 0;\n\n\t\t\t\t\t\tauto Stop = Directive.Args.find(\"stop\");\n\t\t\t\t\t\tif (Stop != Directive.Args.end())\n\t\t\t\t\t\t{\n\t\t\t\t\t\t\tStringify::ToLower(Threads->second);\n\t\t\t\t\t\t\tauto Value = FromString<uint8_t>(Threads->second);\n\t\t\t\t\t\t\tif (!Value)\n\t\t\t\t\t\t\t\tEnv.AutoStop = (Threads->second == \"on\" || Threads->second == \"true\" || Threads->second == \"yes\");\n\t\t\t\t\t\t\telse\n\t\t\t\t\t\t\t\tEnv.AutoStop = *Value > 0;\n\t\t\t\t\t\t}\n\t\t\t\t\t}\n\t\t\t\t\telse if (Directive.Name == \"#console::main\")\n\t\t\t\t\t\tEnv.AutoConsole = true;\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\t};\n}\n#endif";
		callback(context, "executable/runtime.hpp", dc_executable_runtime_hpp.c_str(), (unsigned int)dc_executable_runtime_hpp.size());

		const char* sc_executable_vcpkg_json = "{\n    \"name\": \"{{BUILDER_OUTPUT}}\",\n    \"description\": \"Program: {{BUILDER_OUTPUT}}\",\n    \"version\": \"1.0.0\",\n    \"builtin-baseline\": \"e038ef04796ee67814f36af7c235ae50bbdf4303\",\n    \"dependencies\": {{BUILDER_CONFIG_INSTALL}}\n}";
		callback(context, "executable/vcpkg.json", sc_executable_vcpkg_json, 226);
	}
}
#endif