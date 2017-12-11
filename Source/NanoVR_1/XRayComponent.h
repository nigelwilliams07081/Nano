// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ReciprocalLatticeComponent.h"
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
	void InitializeXRayComponents();
	void MoveRadially(float movementSpeed);
	void ChangeParticleSystemRotation(float rotationAmount);
	void AddDebugTextToScreen(FString text, float duration);
	void FindAngleBetweenLocations(FVector& currentLocation, FVector& lastLocation);
	void FindTotalAngleFromOrigin(FVector& currentLocation);
	float GetAngleBetweenVectors(FVector& v1, FVector& v2);
	void ManageInputs(float& deltaTime);
	void ManageBraggTheorem(float theta);

	UFUNCTION()
	void DestroyParticle();
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere)
		UParticleSystem* m_ParticleSystemPrefab;
	UPROPERTY(EditAnywhere)
		float m_DistanceFromLattice;
	UPROPERTY(EditAnywhere)
		float m_VerticalRotationSpeed;
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Wavelength (in nanometers)", ClampMin = 0.01, ClampMax = 10.0))
		float m_Wavelength;

	UParticleSystemComponent* m_ParticleSystem;
	//TArray<UParticleSystemComponent*> m_ParticleSystemList;
	APlayerController* m_PlayerController;

	float m_Velocity;
	float m_Rotation;
	bool m_IsParticleSystemActive = true;
	float m_OriginalAngle;
	float m_ChangeInAngle;
	float m_Angle;

	FVector m_LocationOrigin;

	FVector m_OriginalActorVector;

	FVector m_OriginalActorLocation;
	FVector m_LastLocation;
	FVector m_CurrentLocation;
	UReciprocalLatticeComponent* m_ReciprocalLattice;

	float m_TwoDSinTheta_Rounded;
	float m_Wavelength_Rounded;

	bool m_LatticeHasInitialized;
};
