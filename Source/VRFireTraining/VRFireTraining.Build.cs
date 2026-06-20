// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class VRFireTraining : ModuleRules
{
    public VRFireTraining(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "HeadMountedDisplay",
            "AIModule",             // AIController.h, FAIMoveRequest
            "NavigationSystem",     // NavMesh, MoveToActor
            "UMG",                  // UUserWidget, UTextBlock
            "Slate",                // FText, widget internals
            "SlateCore"             // ESlateVisibility
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}