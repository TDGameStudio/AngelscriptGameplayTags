#pragma once

ANGELSCRIPTGAMEPLAYTAGS_API void AngelscriptReloadGameplayTags();

// Re-registers every cached GameplayTag as a `GameplayTags::<sanitized>` global variable
// on the *currently scoped* asIScriptEngine if that engine has not already received
// the namespace globals. Use from automation tests that acquire a fresh test engine
// on which startup binds have not registered the GameplayTag namespace globals.
ANGELSCRIPTGAMEPLAYTAGS_API void AngelscriptRebindGameplayTagsToCurrentEngine();
