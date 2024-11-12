// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class wunthshinEditorModule : ModuleRules
{
	public wunthshinEditorModule(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine"  });

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"UnrealEd",
				"EditorSubsystem",
				"wunthshin"
			});
		}
	}
}
