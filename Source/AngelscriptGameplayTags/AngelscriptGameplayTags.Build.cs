using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
	public class AngelscriptGameplayTags : ModuleRules
	{
		public AngelscriptGameplayTags(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicDependencyModuleNames.AddRange(new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"AngelscriptRuntime",
				"GameplayTags",
			});
		}
	}
}
