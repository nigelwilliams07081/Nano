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
	
	GetOwner()->SetActorLocation(FVector(m_LocationOrigin.X, m_LocationOrigin.Y, m_LocationOrigin.Z + m_DistanceFromLattice));
	GetOwner()->SetActorRotation(FQuat(FRotator(0.0f, 0.0f, 0.0f)));
	m_ParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_ParticleSystemPrefab, FTransform(GetOwner()->GetActorLocation()), false);
	m_ParticleSystem->SetWorldScale3D(FVector(m_Wavelength, m_Wavelength, m_Wavelength));
	// TODO: Allow Particle System to only have 1 Max Collision

	FScriptDelegate Delegate;
	Delegate.BindUFunction(this, "DestroyParticle");
	m_ParticleSystem->OnParticleCollide.AddUnique(Delegate);

	if (m_ParticleSystem->OnParticleCollide.Contains(Delegate))
	{
		AddDebugTextToScreen("Has DestroyParticle method", 1.5f);
		m_ParticleSystem->OnParticleCollide.Remove(Delegate);

		if (!m_ParticleSystem->OnParticleCollide.Contains(Delegate))
		{
			AddDebugTextToScreen("DestroyParticle method Destroyed", 1.5f);
		}
	}

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
	// ...
	
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
		ChangeParticleSystemRotation(m_ChangeInAngle * 180.0f / PI * m_AngleAdjustmentConstant);
	}
	else if (m_PlayerController->IsInputKeyDown(EKeys::K))
	{
		MoveRadially(-m_VerticalRotationSpeed * deltaTime);
		ChangeParticleSystemRotation(-m_ChangeInAngle * 180.0f / PI * m_AngleAdjustmentConstant);
	}
	else if (m_PlayerController->IsInputKeyDown(EKeys::L))
	{
		if (m_ParticleSystem->RelativeScale3D.Size() < 7.0f)
		{
			m_ParticleSystem->SetRelativeScale3D(FVector(m_ParticleSystem->RelativeScale3D.X + deltaTime, m_ParticleSystem->RelativeScale3D.Y + deltaTime, m_ParticleSystem->RelativeScale3D.Z + deltaTime));
		}
	}
	else if (m_PlayerController->IsInputKeyDown(EKeys::J))
	{
		if (m_ParticleSystem->RelativeScale3D.Size() > 1.0f)
		{
			m_ParticleSystem->SetRelativeScale3D(FVector(m_ParticleSystem->RelativeScale3D.X - deltaTime, m_ParticleSystem->RelativeScale3D.Y - deltaTime, m_ParticleSystem->RelativeScale3D.Z - deltaTime));
		}
	}
}

void UXRayComponent::ManageBraggTheorem(float theta)
{
	float Two_D_Sin_Theta = (round(2.0f * m_ReciprocalLattice->GetDistanceBetweenNodes() * FMath::Sin(theta) * 10.0f) / 10.0f);
	if (m_Wavelength == Two_D_Sin_Theta)
	{
		// Allow 2 Max Collisions
		AddDebugTextToScreen(FString("Appear!!" + FString::SanitizeFloat(2.0f * m_ReciprocalLattice->GetDistanceBetweenNodes() * FMath::Sin(theta))), 1.5f);
	}
	else
	{
		// Allow 1 Max Collision
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

	// Find the Reciprocal Lattice in the World
	if (!m_LatticeHasInitialized)
	{
		for (TObjectIterator<UReciprocalLatticeComponent> ObjectItr; ObjectItr; ++ObjectItr)
		{
			m_ReciprocalLattice = *ObjectItr;
		}
		m_LatticeHasInitialized = true;
	}
	
	ManageInputs(DeltaTime);
	ManageBraggTheorem(FMath::Abs(m_Angle));
	
	m_ParticleSystem->SetWorldRotation(FQuat(GetOwner()->GetActorRotation()));
	m_ParticleSystem->SetWorldLocation(GetOwner()->GetActorLocation());
	// ...
}