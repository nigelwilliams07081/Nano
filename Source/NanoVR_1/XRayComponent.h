// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReciprocalLatticeComponent.h"
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
	void RotateVertical(float rotationAmount);
	void AddDebugTextToScreen(FString text);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//UParticleSystemComponent* GetParticleSystem() const;
private:
	UPROPERTY(EditAnywhere)
		UParticleSystem* m_ParticleSystemPrefab;
	UPROPERTY(EditAnywhere)
		FVector m_WorldLocation;
	UPROPERTY(EditAnywhere)
		float m_VerticalRotationSpeed;

	//UReciprocalLatticeComponent* m_Lattice;
	//UParticleSystemComponent* m_ParticleSystem;
	TArray<UParticleSystemComponent*> m_ParticleSystemList;
	APlayerController* m_PlayerController;

	FVector m_ParticleSystemVelocity;
	bool m_IsParticleSystemActive = true;
	
};
