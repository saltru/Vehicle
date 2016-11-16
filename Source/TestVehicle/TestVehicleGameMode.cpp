// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TestVehicle.h"
#include "TestVehicleGameMode.h"
#include "TestVehiclePawn.h"
#include "TestVehicleHud.h"

ATestVehicleGameMode::ATestVehicleGameMode()
{
	DefaultPawnClass = ATestVehiclePawn::StaticClass();
	HUDClass = ATestVehicleHud::StaticClass();
}
