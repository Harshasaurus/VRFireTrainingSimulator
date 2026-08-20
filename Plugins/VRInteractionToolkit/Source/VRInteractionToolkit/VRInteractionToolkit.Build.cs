using UnrealBuildTool;

public class VRInteractionToolkit : ModuleRules
{
	public VRInteractionToolkit(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[] { ModuleDirectory + "/Public" });
		PrivateIncludePaths.AddRange(new string[] { ModuleDirectory + "/Private" });

		PublicDependencyModuleNames.AddRange(new string[]
		{
         "Core",
		 "CoreUObject",
		  "Engine",
		 "InputCore",
		 "EnhancedInput",
		 "HeadMountedDisplay"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
