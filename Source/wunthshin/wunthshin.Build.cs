// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class wunthshin : ModuleRules
{
	public wunthshin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "AIModule", "NavigationSystem", "LevelSequence"  });

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"UnrealEd",
				"EditorSubsystem",
				"wunthshinEditorModule"
			});
		}
	}
}
