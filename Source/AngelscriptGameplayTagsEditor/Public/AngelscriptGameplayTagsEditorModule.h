#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ANGELSCRIPTGAMEPLAYTAGSEDITOR_API FAngelscriptGameplayTagsEditorModule : public FDefaultModuleImpl
{
public:
	void StartupModule() override;
	void ShutdownModule() override;

private:
#if WITH_DEV_AUTOMATION_TESTS
	friend struct FAngelscriptGameplayTagsEditorModuleTestAccess;
#endif

	void RegisterGameplayTagDelegates();
	void UnregisterGameplayTagDelegates();
	void ReloadTags();
};

#if WITH_DEV_AUTOMATION_TESTS
struct ANGELSCRIPTGAMEPLAYTAGSEDITOR_API FAngelscriptGameplayTagsEditorModuleTestAccess
{
	static void SetReloadGameplayTagsOverride(TFunction<void(FAngelscriptGameplayTagsEditorModule*)> InOverride);
	static void ResetReloadGameplayTagsOverride();
	static void RegisterGameplayTagDelegates(FAngelscriptGameplayTagsEditorModule& Module);
};
#endif
