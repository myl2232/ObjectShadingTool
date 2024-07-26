// Copyright (c) 2022 YDrive Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ShadingStyle.h"


class FShadingToolCommands : public TCommands<FShadingToolCommands>
{
public:
	FShadingToolCommands()
		: TCommands<FShadingToolCommands>(
			TEXT("ShadingTools"),
			NSLOCTEXT("Contexts", "ShadingTools", "ShadingTools Plugin"),
			NAME_None,
			FShadingStyle::GetStyleSetName()) {}

	// TCommands<> interface
	void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};
