// (C) 2019 TinyGoose Ltd., All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PBVRTarget : TargetRules
{
	public PBVRTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "PBVR" } );
	}
}
