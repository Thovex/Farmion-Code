// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FarmionRem : ModuleRules
{
	public FarmionRem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Chaos",
			"GeometryCollectionEngine",
			"Json",
			"AIModule",
			"UMG",
			"Slate",
			"SlateCore",
			"NavigationSystem",
			"ActorSequence",
			"EnhancedInput",
			"AudioWidgets",
		});

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"OnlineSubsystem",
			"CoreUObject", 
			"OnlineSubsystemUtils", 
			"Networking", 
			"Sockets", 
			"Voice",
			"OnlineSubsystemSteam",
			"AdvancedSessions",
			"AdvancedSteamSessions",
			"Steamworks",
			"SteamSockets",

		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"OnlineSubsystem", 
			"Sockets", 
			"Networking", 
			"OnlineSubsystemUtils", 
			"Voice", 
			"Steamworks",
			"OnlineSubsystemSteam",
			"CommonUI",
			"CommonInput",
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}