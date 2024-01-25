#ifndef RUNTIME_H
#define RUNTIME_H
#include <vitex/core/scripting.h>

using namespace Vitex::Core;
using namespace Vitex::Compute;
using namespace Vitex::Scripting;

namespace ASX
{
	enum class ExitStatus
	{
		Continue = 0x00fffff - 1,
		OK = 0,
		RuntimeError,
		PrepareError,
		LoadingError,
		SavingError,
		CompilerError,
		EntrypointError,
		InputError,
		InvalidCommand,
		InvalidDeclaration,
		CommandError,
		Kill
	};

	struct ProgramEntrypoint
	{
		const char* ReturnsWithArgs = "int main(array<string>@)";
		const char* Returns = "int main()";
		const char* Simple = "void main()";
	};

	struct EnvironmentConfig
	{
		OS::Process::ArgsContext Params;
		UnorderedSet<String> Addons;
		Vector<String> Args;
		FunctionDelegate AtExit;
		FileEntry File;
		String Name;
		String Path;
		String Program;
		String Registry;
		String Mode;
		String Output;
		String Addon;
		Compiler* ThisCompiler;
		const char* Module;
		bool Inline;

		EnvironmentConfig(int ArgsCount, char** ArgsData) : Params(ArgsCount, ArgsData), ThisCompiler(nullptr), Module("__anonymous__"), Inline(true)
		{
			Args.reserve((size_t)ArgsCount);
			for (int i = 0; i < ArgsCount; i++)
				Args.push_back(ArgsData[i]);
		}
		static EnvironmentConfig& Get(EnvironmentConfig* Other = nullptr)
		{
			static EnvironmentConfig* Base = Other;
			VI_ASSERT(Base != nullptr, "env was not set");
			return *Base;
		}
	};

	struct SystemConfig
	{
		UnorderedMap<String, std::pair<String, String>> Functions;
		Vector<std::pair<String, bool>> Libraries;
		Vector<std::pair<String, int32_t>> Settings;
		Vector<String> SystemAddons;
		bool TsImports = true;
		bool Addons = true;
		bool CLibraries = true;
		bool CFunctions = true;
		bool Files = true;
		bool Remotes = true;
		bool Debug = false;
		bool Translator = false;
		bool Interactive = false;
		bool EssentialsOnly = true;
		bool LoadByteCode = false;
		bool SaveByteCode = false;
		bool SaveSourceCode = false;
		bool Dependencies = false;
		bool Install = false;
		size_t Installed = 0;
	};

	class Runtime
	{
	public:
		static bool ConfigureContext(SystemConfig& Config, EnvironmentConfig& Env, VirtualMachine* VM, Compiler* ThisCompiler)
		{
			uint32_t ImportOptions = 0;
			if (Config.Addons)
				ImportOptions |= (uint32_t)Imports::Addons;
			if (Config.CLibraries)
				ImportOptions |= (uint32_t)Imports::CLibraries;
			if (Config.CFunctions)
				ImportOptions |= (uint32_t)Imports::CFunctions;
			if (Config.Files)
				ImportOptions |= (uint32_t)Imports::Files;
			if (Config.Remotes)
				ImportOptions |= (uint32_t)Imports::Remotes;

			VM->SetTsImports(Config.TsImports);
			VM->SetModuleDirectory(OS::Path::GetDirectory(Env.Path.c_str()));
			VM->SetPreserveSourceCode(Config.SaveSourceCode);
			VM->SetImports(ImportOptions);

			if (Config.Translator)
				VM->SetByteCodeTranslator((uint32_t)TranslationOptions::Optimal);

			for (auto& Name : Config.SystemAddons)
			{
				if (!VM->ImportSystemAddon(Name))
				{
					VI_ERR("system addon <%s> cannot be loaded", Name.c_str());
					return false;
				}
			}

			for (auto& Path : Config.Libraries)
			{
				if (!VM->ImportCLibrary(Path.first, Path.second))
				{
					VI_ERR("external %s <%s> cannot be loaded", Path.second ? "addon" : "clibrary", Path.first.c_str());
					return false;
				}
			}

			for (auto& Data : Config.Functions)
			{
				if (!VM->ImportCFunction({ Data.first }, Data.second.first, Data.second.second))
				{
					VI_ERR("clibrary function <%s> from <%s> cannot be loaded", Data.second.first.c_str(), Data.first.c_str());
					return false;
				}
			}

			auto* Macro = ThisCompiler->GetProcessor();
			Macro->AddDefaultDefinitions();

			Env.ThisCompiler = ThisCompiler;
			EnvironmentConfig::Get(&Env);

			VM->ImportSystemAddon("ctypes");
			VM->BeginNamespace("this_process");
			VM->SetFunctionDef("void exit_event(int)");
			VM->SetFunction("void before_exit(exit_event@)", &Runtime::ApplyContextExit);
			VM->SetFunction("uptr@ get_compiler()", &Runtime::GetCompiler);
			VM->EndNamespace();
			return true;
		}
		static bool TryContextExit(EnvironmentConfig& Env, int Value)
		{
			if (!Env.AtExit.IsValid())
				return false;

			auto Status = Env.AtExit([Value](ImmediateContext* Context)
			{
				Context->SetArg32(0, Value);
			}).Get();
			Env.AtExit.Release();
			VirtualMachine::CleanupThisThread();
			return !!Status;
		}
		static void ApplyContextExit(asIScriptFunction* Callback)
		{
			auto& Env = EnvironmentConfig::Get();
			ImmediateContext* Context = Callback ? Env.ThisCompiler->GetVM()->RequestContext() : nullptr;
			Env.AtExit = FunctionDelegate(Callback, Context);
			VI_RELEASE(Context);
		}
		static void AwaitContext(Schedule* Queue, EventLoop* Loop, VirtualMachine* VM, ImmediateContext* Context)
		{
			EventLoop::Set(Loop);
			while (Loop->PollExtended(Context, 1000))
				Loop->Dequeue(VM);

			while (!Queue->CanEnqueue() && Queue->HasAnyTasks())
				Queue->Dispatch();

			Queue->Stop();
			EventLoop::Set(nullptr);
			Context->Reset();
			VM->PerformFullGarbageCollection();
			ApplyContextExit(nullptr);
		}
		static Function GetEntrypoint(EnvironmentConfig& Env, ProgramEntrypoint& Entrypoint, Compiler* Unit)
		{
			Function MainReturnsWithArgs = Unit->GetModule().GetFunctionByDecl(Entrypoint.ReturnsWithArgs);
			Function MainReturns = Unit->GetModule().GetFunctionByDecl(Entrypoint.Returns);
			Function MainSimple = Unit->GetModule().GetFunctionByDecl(Entrypoint.Simple);
			if (MainReturnsWithArgs.IsValid() || MainReturns.IsValid() || MainSimple.IsValid())
				return MainReturnsWithArgs.IsValid() ? MainReturnsWithArgs : (MainReturns.IsValid() ? MainReturns : MainSimple);

			VI_ERR("module %s must contain either: <%s>, <%s> or <%s>", Env.Module, Entrypoint.ReturnsWithArgs, Entrypoint.Returns, Entrypoint.Simple);
			return Function(nullptr);
		}
		static Compiler* GetCompiler()
		{
			return EnvironmentConfig::Get().ThisCompiler;
		}
	};
}
#endif