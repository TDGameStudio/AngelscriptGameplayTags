#include "AngelscriptGameplayTagsEditorModule.h"

#include "AngelscriptGameplayTagsBinds.h"
#include "Core/AngelscriptEngine.h"
#include "GameplayTagsModule.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FAngelscriptGameplayTagsEditorModule, AngelscriptGameplayTagsEditor);

namespace
{
#if WITH_DEV_AUTOMATION_TESTS
	TFunction<void(FAngelscriptGameplayTagsEditorModule*)> GReloadGameplayTagsOverrideForTesting;
#endif
}

#if WITH_DEV_AUTOMATION_TESTS
void FAngelscriptGameplayTagsEditorModuleTestAccess::SetReloadGameplayTagsOverride(TFunction<void(FAngelscriptGameplayTagsEditorModule*)> InOverride)
{
	GReloadGameplayTagsOverrideForTesting = MoveTemp(InOverride);
}

void FAngelscriptGameplayTagsEditorModuleTestAccess::ResetReloadGameplayTagsOverride()
{
	GReloadGameplayTagsOverrideForTesting = nullptr;
}

void FAngelscriptGameplayTagsEditorModuleTestAccess::RegisterGameplayTagDelegates(FAngelscriptGameplayTagsEditorModule& Module)
{
	Module.RegisterGameplayTagDelegates();
}
#endif

void FAngelscriptGameplayTagsEditorModule::StartupModule()
{
	RegisterGameplayTagDelegates();
}

void FAngelscriptGameplayTagsEditorModule::ShutdownModule()
{
	UnregisterGameplayTagDelegates();
}

void FAngelscriptGameplayTagsEditorModule::RegisterGameplayTagDelegates()
{
	IGameplayTagsModule::OnTagSettingsChanged.RemoveAll(this);
	IGameplayTagsModule::OnGameplayTagTreeChanged.RemoveAll(this);

	IGameplayTagsModule::OnTagSettingsChanged.AddRaw(this, &FAngelscriptGameplayTagsEditorModule::ReloadTags);
	IGameplayTagsModule::OnGameplayTagTreeChanged.AddRaw(this, &FAngelscriptGameplayTagsEditorModule::ReloadTags);
}

void FAngelscriptGameplayTagsEditorModule::UnregisterGameplayTagDelegates()
{
	IGameplayTagsModule::OnTagSettingsChanged.RemoveAll(this);
	IGameplayTagsModule::OnGameplayTagTreeChanged.RemoveAll(this);
}

void FAngelscriptGameplayTagsEditorModule::ReloadTags()
{
#if WITH_DEV_AUTOMATION_TESTS
	if (GReloadGameplayTagsOverrideForTesting)
	{
		GReloadGameplayTagsOverrideForTesting(this);
		return;
	}
#endif

	FAngelscriptEngine* CurrentEngine = FAngelscriptEngine::TryGetCurrentEngine();
	if (CurrentEngine == nullptr)
	{
		return;
	}

	FAngelscriptEngineScope EngineScope(*CurrentEngine);
	AngelscriptReloadGameplayTags();
}
