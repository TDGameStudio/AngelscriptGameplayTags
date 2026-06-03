using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
	public class AngelscriptGameplayTagsEditor : ModuleRules
	{
		public AngelscriptGameplayTagsEditor(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicDependencyModuleNames.AddRange(new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"AngelscriptRuntime",
				"AngelscriptGameplayTags",
				"GameplayTags",
			});
		}
	}
}
