// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HideAndSeekWithAI : ModuleRules
{
	public HideAndSeekWithAI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"AIModule",
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Slate",
			"SlateCore",
			"UMG",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        bool bDebugBuild = Target.Configuration == UnrealTargetConfiguration.Debug
            || Target.Configuration == UnrealTargetConfiguration.DebugGame;
        bool bShippingBuild = Target.Configuration == UnrealTargetConfiguration.Shipping;

        bUseRTTI = false;
        bEnableExceptions = false;
        bUseAVX = true;
        ShadowVariableWarningLevel = WarningLevel.Error;
        bEnableUndefinedIdentifierWarnings = true;
        bUseUnity = bDebugBuild;
        OptimizeCode = bDebugBuild ? CodeOptimization.Never : CodeOptimization.Always;

        if (!bShippingBuild) {
		    PrivateDefinitions.Add("HIDEANDSEEKWITHAI_LOGGING");
        }
	}
}
