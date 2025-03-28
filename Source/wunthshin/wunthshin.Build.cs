// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class wunthshin : ModuleRules
{
	public wunthshin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] 
		{
			"$(ProjectDir)/DBServer/Public"
		});
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "AIModule", "NavigationSystem", "LevelSequence",
			"FCTween" ,"MovieScene", "OnlineSubsystemUtils"
        });

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
