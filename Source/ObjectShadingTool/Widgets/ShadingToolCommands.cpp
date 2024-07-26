// Copyright (c) 2022 YDrive Inc. All rights reserved.

#include "ShadingToolCommands.h"

#define LOCTEXT_NAMESPACE "FShadingToolModule"

void FShadingToolCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "ShadingTool", "Bring up ShadingTool window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
