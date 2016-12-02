// Fill out your copyright notice in the Description page of Project Settings.

#include "TestVehicle.h"
#include "RocketTurret.h"
#include "Engine.h"

// Sets default values for this component's properties
URocketTurret::URocketTurret()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URocketTurret::BeginPlay()
{
	Super::BeginPlay();
	

	
	// ...
	
}


// Called every frame
void URocketTurret::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

