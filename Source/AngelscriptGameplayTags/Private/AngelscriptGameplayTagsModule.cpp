#include "AngelscriptGameplayTagsInternal.h"

#include "Core/AngelscriptEngine.h"
#include "Core/AngelscriptEngineExtensionRegistry.h"
#include "Modules/ModuleManager.h"

class FAngelscriptGameplayTagsExtension final : public IAngelscriptExtension
{
public:
	void OnEngineAttached(FAngelscriptEngine& Engine) override
	{
		FAngelscriptEngineScope EngineScope(Engine);
		AngelscriptGameplayTagsReplayCurrentEngine();
	}

	void OnEngineDetached(FAngelscriptEngine& Engine) override
	{
		AngelscriptGameplayTagsDetachEngine(Engine);
	}
};

class FAngelscriptGameplayTagsModule final : public IModuleInterface
{
public:
	void StartupModule() override
	{
		Extension = MakeShared<FAngelscriptGameplayTagsExtension>();
		ExtensionHandle = FAngelscriptEngineExtensionRegistry::Get().RegisterExtension(Extension.ToSharedRef());
	}

	void ShutdownModule() override
	{
		FAngelscriptEngineExtensionRegistry::Get().UnregisterExtension(ExtensionHandle);
		ExtensionHandle.Reset();
		Extension.Reset();
	}

private:
	TSharedPtr<FAngelscriptGameplayTagsExtension> Extension;
	FDelegateHandle ExtensionHandle;
};

IMPLEMENT_MODULE(FAngelscriptGameplayTagsModule, AngelscriptGameplayTags);
