using UnrealBuildTool;

public class VRFireSimulationToolkit : ModuleRules
{
	public VRFireSimulationToolkit(ReadOnlyTargetRules Target) : base(Target)
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
			"HeadMountedDisplay",
			"NavigationSystem",       // if fire spread / NavMesh queries are needed later
			"VRInteractionToolkit",    // for AVRGrabbable base class
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
