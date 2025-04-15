// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PLAI : ModuleRules
{
	public PLAI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem",
			"AIModule", "Niagara", "EnhancedInput"
		});

		PublicIncludePaths.AddRange(new[]
		{
			"PLAI/Public/Battle/BattlePlayer",
			"PLAI/Public/Battle/Tile"
		});
	}
}