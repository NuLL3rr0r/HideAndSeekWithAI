// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class HideAndSeekWithAIEditorTarget : TargetRules
{
	public HideAndSeekWithAIEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
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
