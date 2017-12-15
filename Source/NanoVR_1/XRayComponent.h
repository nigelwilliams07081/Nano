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
	// Constructor
	UXRayComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void InitializeXRayComponents();

	void MoveRadially(float movementAmount);

	void ChangeParticleSystemRotation(float rotationAmount);

	void AddDebugTextToScreen(FString text, float duration);

	void FindAngleBetweenLocations(FVector& currentLocation, FVector& lastLocation);

	void FindTotalAngleFromOrigin(FVector& currentLocation);

	float DotProduct(FVector& v1, FVector& v2);

	float GetAngleBetweenVectors(FVector& v1, FVector& v2);

	FVector GetVectorFromTwoPoints(FVector& head, FVector& tail);

	void ManageInputs(float& deltaTime);

	void ManageBraggTheorem(float theta);

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
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Is Reciprocal Beam?"))
		bool m_IsReciprocalBeam;

	float RoundToTenth(float valueToRound);

	// The Particle System being used as the X-Ray beam
	UParticleSystemComponent* m_ParticleSystem;

	// The Controller for receiving input
	APlayerController* m_PlayerController;

	// Used to store total value continually added from MoveRadially's @movementAmount parameter
	float m_RadialMovementValue;

	// Used to store total input value for the ChangeParticleSystemRotation() method utilizing FRotator
	float m_RotationValue;

	// Used to store the change in angle of the beam as it rotates around m_LocationOrigin
	float m_ChangeInAngle;

	// Used to store the total angle of the beam as it rotates around m_LocationOrigin
	float m_Angle;

	// The Central point around which the AActor and m_ParticleSystem are rotating
	FVector m_LocationOrigin;

	// Stores the last FVector location of the AActor from the last frame
	FVector m_LastLocation;

	// Stores the current FVector location of the AActor from the current frame
	FVector m_CurrentLocation;

	// Used to retrieve information from the Real-space and Reciprocal-space Lattices
	UReciprocalLatticeComponent* m_ReciprocalLattice;

	// Used to store the rounded value of the TwoDSinTheta calculation used in the ManageBraggTheorem() method
	float m_TwoDSinTheta_Rounded;

	// Used to store the rounded value of the current m_Wavelength
	float m_Wavelength_Rounded;

	// Used to guarantee the InitializeXRayComponents() method is called only once in TickComponent()
	bool m_IsXRayComponentInitialized;
};
