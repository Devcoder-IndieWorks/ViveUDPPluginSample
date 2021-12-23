// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ViveUDPPlugin : ModuleRules
{
	public ViveUDPPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = false;
		
		PrivateIncludePaths.AddRange(
		    new string[] {
				"ViveUDPPlugin/Private"
			});

		PrivateDependencyModuleNames.AddRange(
		    new string[] {
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore", 
                "Sockets",
                "Networking", 
                "Slate", 
                "SlateCore"
			});
	}
}
