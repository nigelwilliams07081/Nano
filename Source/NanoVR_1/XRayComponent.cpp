// Fill out your copyright notice in the Description page of Project Settings.

#include "XRayComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"


// Sets default values for this component's properties
UXRayComponent::UXRayComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_ParticleSystem, GetWorld()->)
		
	// ...
}


// Called when the game starts
void UXRayComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UXRayComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

