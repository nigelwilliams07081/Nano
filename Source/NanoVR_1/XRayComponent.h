// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "XRayComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NANOVR_1_API UXRayComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UXRayComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
		UParticleSystem* m_ParticleSystemPrefab;
	UPROPERTY(EditAnywhere)
		FVector m_WorldLocation;

	UParticleSystemComponent* m_ParticleSystem;
	
};
