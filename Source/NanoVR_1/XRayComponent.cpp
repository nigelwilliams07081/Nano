// Fill out your copyright notice in the Description page of Project Settings.

#include "XRayComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"
#include <string>

// Sets default values for this component's properties
UXRayComponent::UXRayComponent() : m_LatticeHasInitialized(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}


// Called when the game starts
void UXRayComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
	// ...
}

void UXRayComponent::InitializeXRayComponents()
{
	for (TObjectIterator<UReciprocalLatticeComponent> ObjectItr; ObjectItr; ++ObjectItr)
	{
		m_ReciprocalLattice = *ObjectItr;
	}

	m_LocationOrigin = m_ReciprocalLattice->GetWorldLocation();
	m_LocationOrigin.Z += m_ReciprocalLattice->GetDistanceBetweenNodesZ() * (m_ReciprocalLattice->GetNumberOfNodesZ() - 1);
	GetOwner()->SetActorLocation(FVector(m_LocationOrigin.X, m_LocationOrigin.Y, m_LocationOrigin.Z + m_DistanceFromLattice));
	GetOwner()->SetActorRotation(FQuat(FRotator(0.0f, 0.0f, 0.0f)));
	m_ParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_ParticleSystemPrefab, FTransform(GetOwner()->GetActorLocation()), false);
	m_ParticleSystem->SetWorldScale3D(FVector(m_Wavelength, m_Wavelength, m_Wavelength));

	m_ParticleSystem->SetFloatParameter("MaxCollisions", 1.0f);

	m_PlayerController = GetWorld()->GetFirstPlayerController();
	m_Velocity = 0.0f;
	m_Rotation = 0.0f;

	m_OriginalAngle = GetOwner()->GetActorRotation().Pitch;
	m_ChangeInAngle = 0.0f;

	// Set all of Location variables to be the current location of the actor
	m_CurrentLocation = GetOwner()->GetActorLocation();
	m_LastLocation = m_CurrentLocation;
	m_OriginalActorLocation = m_CurrentLocation;

	m_OriginalActorVector = FVector(m_OriginalActorLocation - m_LocationOrigin);
}

void UXRayComponent::MoveRadially(float movementSpeed)
{
	m_Velocity += movementSpeed;
	GetOwner()->SetActorLocation(FVector(FMath::Sin(m_Velocity) * m_DistanceFromLattice, 0.0f, (FMath::Cos(m_Velocity) * m_DistanceFromLattice) + m_LocationOrigin.Z));

	m_CurrentLocation = GetOwner()->GetActorLocation();
	FindAngleBetweenLocations(m_CurrentLocation, m_LastLocation);
	FindTotalAngleFromOrigin(m_CurrentLocation);
	m_LastLocation = m_CurrentLocation;
}

void UXRayComponent::ChangeParticleSystemRotation(float rotationAmount)
{
	m_Rotation += -rotationAmount;
	GetOwner()->SetActorRotation(FQuat(FRotator(m_Rotation, GetOwner()->GetActorRotation().Yaw, GetOwner()->GetActorRotation().Roll)));
}

void UXRayComponent::AddDebugTextToScreen(FString text, float duration)
{
	if (GEngine != NULL)
	{
		GEngine->AddOnScreenDebugMessage(0, duration, FColor::Yellow, text);
	}
}

void UXRayComponent::FindAngleBetweenLocations(FVector& currentLocation, FVector& lastLocation)
{
	FVector lastVector = FVector(lastLocation - m_LocationOrigin);
	FVector currentVector = FVector(currentLocation - m_LocationOrigin);

	m_ChangeInAngle = GetAngleBetweenVectors(lastVector, currentVector);
}

void UXRayComponent::FindTotalAngleFromOrigin(FVector & currentLocation)
{
	FVector currentVector = FVector(currentLocation - m_LocationOrigin);

	m_Angle = GetAngleBetweenVectors(m_OriginalActorVector, currentVector);
}

float UXRayComponent::GetAngleBetweenVectors(FVector & v1, FVector & v2)
{
	return FMath::Acos(((v1.X * v2.X) + (v1.Y * v2.Y) + (v1.Z * v2.Z)) / (v1.Size() * v2.Size()));
}

void UXRayComponent::ManageInputs(float & deltaTime)
{
	if (m_PlayerController->IsInputKeyDown(EKeys::I))
	{
		MoveRadially(m_VerticalRotationSpeed * deltaTime);
		ChangeParticleSystemRotation(m_ChangeInAngle * 180.0f / PI);
	}
	else if (m_PlayerController->IsInputKeyDown(EKeys::K))
	{
		MoveRadially(-m_VerticalRotationSpeed * deltaTime);
		ChangeParticleSystemRotation(-m_ChangeInAngle * 180.0f / PI);
	}
	else if (m_PlayerController->IsInputKeyDown(EKeys::L))
	{
		if (m_Wavelength < 7.0f)
		{
			m_Wavelength += deltaTime;
		}
	}
	else if (m_PlayerController->IsInputKeyDown(EKeys::J))
	{
		if (m_Wavelength > 1.0f)
		{
			m_Wavelength -= deltaTime;
		}
	}
}

void UXRayComponent::ManageBraggTheorem(float theta)
{
	m_TwoDSinTheta_Rounded = (round(2.0f * m_ReciprocalLattice->GetDistanceBetweenNodesZ() * FMath::Sin(theta) * 10.0f) / 10.0f);
	m_Wavelength_Rounded = (round(m_Wavelength * 10.0f) / 10.0f);
	AddDebugTextToScreen(m_Wavelength_Rounded == m_TwoDSinTheta_Rounded ?
		FString::SanitizeFloat(m_Wavelength_Rounded) + " = " + FString::SanitizeFloat(m_TwoDSinTheta_Rounded) + "!!" :
		FString::SanitizeFloat(m_Wavelength_Rounded) + " != " + FString::SanitizeFloat(m_TwoDSinTheta_Rounded) + "...", 1.0f);

	if (m_Wavelength_Rounded == m_TwoDSinTheta_Rounded)
	{
		// Allow 2 Max Collisions
		m_ParticleSystem->SetFloatParameter("MaxCollisions", 2.0f);
	}
	else
	{
		// Allow 1 Max Collision
		m_ParticleSystem->SetFloatParameter("MaxCollisions", 1.0f);
	}
	
}

void UXRayComponent::DestroyParticle()
{
	AddDebugTextToScreen("Destroy Particle", 1.5f);
}

// Called every frame
void UXRayComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// This has to be done in order for the m_ReciprocalLattice's components to be initialized
	if (UGameplayStatics::GetRealTimeSeconds(this) > 2.0f)
	{
		if (!m_LatticeHasInitialized)
		{
			InitializeXRayComponents();

			m_LatticeHasInitialized = true;
		}
		else
		{
			ManageInputs(DeltaTime);
			ManageBraggTheorem(FMath::Abs(m_Angle));

			m_ParticleSystem->SetWorldScale3D(FVector(m_Wavelength, m_Wavelength, m_Wavelength));
			m_ParticleSystem->SetWorldRotation(FQuat(GetOwner()->GetActorRotation()));
			m_ParticleSystem->SetWorldLocation(GetOwner()->GetActorLocation());
		}
	}
	
	// ...
}