// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TestVehicle.h"
#include "TestVehicleWheelRear.h"
#include "Vehicles/TireType.h"

UTestVehicleWheelRear::UTestVehicleWheelRear()
{
	ShapeRadius = 57.f;
	ShapeWidth = 30.0f;
	bAffectedByHandbrake = true;
	SteerAngle = 0.f;
	DampingRate = 0.25f;

	TireType = LoadObject<UTireType>(NULL, TEXT("/Game/Vehicles/VH_Buggy/Blueprint/VH_Buggy_TireType.VH_Buggy_TireType"), NULL, LOAD_None, NULL);

	LatStiffMaxLoad = 4.0f;
	LatStiffValue = 10.0f;
	LongStiffValue = 10000.0f;

	SuspensionMaxRaise = 20.0f;
	SuspensionMaxDrop = 40.0f;
	SuspensionNaturalFrequency = 10.0f;
	SuspensionDampingRatio = 1.1f;

	MaxBrakeTorque = 4000.0f;
	MaxHandBrakeTorque = 6000.0f;
}
