// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "TestVehicleHud.generated.h"


UCLASS(config = Game)
class ATestVehicleHud : public AHUD
{
	GENERATED_BODY()

public:
	ATestVehicleHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface
};
