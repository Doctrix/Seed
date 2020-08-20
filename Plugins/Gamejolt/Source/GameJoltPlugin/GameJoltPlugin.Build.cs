using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules
{
	public class GameJoltPlugin : ModuleRules
	{
        public GameJoltPlugin(ReadOnlyTargetRules Target) : base (Target)
		{

			PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Classes"));

			PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
                    "HTTP",
                    "CoreUObject",
                    "Engine",
                    "Json",
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Engine",
                    "Core",
                    "CoreUObject",
                    "HTTP",
                    "JSON",
				}
				);
		}
	}
}