// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class HideAndSeekWithAITarget : TargetRules
{
	public HideAndSeekWithAITarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "HideAndSeekWithAI" } );

		BuildEnvironment = TargetBuildEnvironment.Shared;

		bEnforceIWYU = true;
		bForceEnableRTTI = false;
		bForceEnableExceptions = false;
		bUseUnityBuild = true;
		bForceUnityBuild = true;
	}
}
