using System.IO;
using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
	public class AngelscriptGameplayTagsTest : ModuleRules
	{
		public AngelscriptGameplayTagsTest(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PrivateIncludePaths.Add(Path.Combine(PluginDirectory, "..", "Angelscript", "Source", "AngelscriptTest"));
			PrivateIncludePaths.Add(Path.Combine(PluginDirectory, "..", "Angelscript", "Source", "AngelscriptTest", "Shared"));

			PublicDependencyModuleNames.AddRange(new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"AngelscriptRuntime",
				"AngelscriptGameplayTags",
				"AngelscriptGameplayTagsEditor",
				"AngelscriptTest",
				"GameplayTags",
			});

			if (Target.bBuildEditor)
			{
				PrivateDependencyModuleNames.AddRange(new string[]
				{
					"CQTest",
					"UnrealEd",
				});
			}
		}
	}
}
