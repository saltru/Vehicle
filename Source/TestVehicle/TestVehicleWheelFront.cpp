// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TestVehicle.h"
#include "TestVehicleWheelFront.h"
#include "Vehicles/TireType.h"

UTestVehicleWheelFront::UTestVehicleWheelFront()
{
	ShapeRadius = 52.f;
	ShapeWidth = 23.0f;
	bAffectedByHandbrake = false;
	SteerAngle = 50.f;
	DampingRate = 0.25f;

	TireType = LoadObject<UTireType>(NULL, TEXT("/Game/Vehicles/VH_Buggy/Blueprint/VH_Buggy_TireType.VH_Buggy_TireType"), NULL, LOAD_None, NULL);

	LatStiffMaxLoad = 5.0f;
	LatStiffValue = 1000.0f;
	LongStiffValue = 1000.0f;

	SuspensionMaxRaise = 20.0f;
	SuspensionMaxDrop = 40.0f;
	SuspensionNaturalFrequency = 10.0f;
	SuspensionDampingRatio = 1.1f;

	MaxBrakeTorque = 4000.0f;
	MaxHandBrakeTorque = 6000.0f;
	
}
