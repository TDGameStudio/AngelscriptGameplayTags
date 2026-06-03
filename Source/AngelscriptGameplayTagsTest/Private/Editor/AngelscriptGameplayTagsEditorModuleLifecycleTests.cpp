#include "AngelscriptGameplayTagsEditorModule.h"

#include "GameplayTagsModule.h"
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAngelscriptGameplayTagsEditorModuleReloadTagsDelegatesLifecycleTest,
	"Angelscript.GameplayTags.Editor.Module.ReloadTagsDelegatesRegisterOnceAndUnbindOnShutdown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAngelscriptGameplayTagsEditorModuleReloadTagsDelegatesLifecycleTest::RunTest(const FString& Parameters)
{
	FAngelscriptGameplayTagsEditorModule Module;
	bool bModuleStarted = false;
	int32 ReloadCallCount = 0;

	FAngelscriptGameplayTagsEditorModuleTestAccess::SetReloadGameplayTagsOverride(
		[&ReloadCallCount, &Module](FAngelscriptGameplayTagsEditorModule* ReloadingModule)
		{
			if (ReloadingModule == &Module)
			{
				++ReloadCallCount;
			}
		});

	ON_SCOPE_EXIT
	{
		if (bModuleStarted)
		{
			Module.ShutdownModule();
		}

		FAngelscriptGameplayTagsEditorModuleTestAccess::ResetReloadGameplayTagsOverride();
	};

	if (!TestFalse(
			TEXT("GameplayTags editor module should start with no tag-settings binding"),
			IGameplayTagsModule::OnTagSettingsChanged.IsBoundToObject(&Module))
		|| !TestFalse(
			TEXT("GameplayTags editor module should start with no tag-tree binding"),
			IGameplayTagsModule::OnGameplayTagTreeChanged.IsBoundToObject(&Module)))
	{
		return false;
	}

	Module.StartupModule();
	bModuleStarted = true;

	if (!TestTrue(
			TEXT("GameplayTags editor module should bind the tag-settings delegate during startup"),
			IGameplayTagsModule::OnTagSettingsChanged.IsBoundToObject(&Module))
		|| !TestTrue(
			TEXT("GameplayTags editor module should bind the tag-tree delegate during startup"),
			IGameplayTagsModule::OnGameplayTagTreeChanged.IsBoundToObject(&Module)))
	{
		return false;
	}

	const int32 ReloadCallCountBeforeStartupBroadcasts = ReloadCallCount;
	IGameplayTagsModule::OnTagSettingsChanged.Broadcast();
	IGameplayTagsModule::OnGameplayTagTreeChanged.Broadcast();
	const int32 ReloadDeltaPerBroadcastPair = ReloadCallCount - ReloadCallCountBeforeStartupBroadcasts;
	if (!TestEqual(
			TEXT("GameplayTags editor module should react to the initial gameplay-tags broadcast pair"),
			ReloadDeltaPerBroadcastPair > 0,
			true))
	{
		return false;
	}

	FAngelscriptGameplayTagsEditorModuleTestAccess::RegisterGameplayTagDelegates(Module);
	const int32 ReloadCallCountBeforeReregisterBroadcasts = ReloadCallCount;
	IGameplayTagsModule::OnTagSettingsChanged.Broadcast();
	IGameplayTagsModule::OnGameplayTagTreeChanged.Broadcast();
	if (!TestEqual(
			TEXT("GameplayTags editor module should keep the same reload delta after re-registering delegates"),
			ReloadCallCount - ReloadCallCountBeforeReregisterBroadcasts,
			ReloadDeltaPerBroadcastPair))
	{
		return false;
	}

	Module.ShutdownModule();
	bModuleStarted = false;

	if (!TestFalse(
			TEXT("GameplayTags editor module should unbind the tag-settings delegate during shutdown"),
			IGameplayTagsModule::OnTagSettingsChanged.IsBoundToObject(&Module))
		|| !TestFalse(
			TEXT("GameplayTags editor module should unbind the tag-tree delegate during shutdown"),
			IGameplayTagsModule::OnGameplayTagTreeChanged.IsBoundToObject(&Module)))
	{
		return false;
	}

	const int32 ReloadCallCountBeforeShutdownBroadcasts = ReloadCallCount;
	IGameplayTagsModule::OnTagSettingsChanged.Broadcast();
	IGameplayTagsModule::OnGameplayTagTreeChanged.Broadcast();
	return TestEqual(
		TEXT("GameplayTags editor module should stop reloading after shutdown"),
		ReloadCallCount - ReloadCallCountBeforeShutdownBroadcasts,
		0);
}

#endif
