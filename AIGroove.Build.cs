// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class AIGroove : ModuleRules
{
	public AIGroove(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "EnhancedInput", "AudioMixer"});

        PrivateDependencyModuleNames.AddRange(new string[] { "HTTP", "Json", "JsonUtilities" });

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PublicDependencyModuleNames.Add("Launch");

            //UPL XML 경로 추가 (UPL 강제 등록)
            string PluginPath = Path.Combine(ModuleDirectory, "AIGroove_UPL.xml");
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", PluginPath);
        }
    }


}
