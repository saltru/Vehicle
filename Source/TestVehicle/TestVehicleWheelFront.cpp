// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TestVehicle.h"
#include "TestVehicleWheelFront.h"

UTestVehicleWheelFront::UTestVehicleWheelFront()
{
	ShapeRadius = 52.f;
	ShapeWidth = 23.0f;
	bAffectedByHandbrake = false;
	SteerAngle = 50.f;
}
