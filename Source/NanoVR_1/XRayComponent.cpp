// Fill out your copyright notice in the Description page of Project Settings.

#include "XRayComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"
#include <string>

UXRayComponent::UXRayComponent() : m_IsXRayComponentInitialized(false)
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
	/*
	Searches through all the UActorComponent objects in the World and returns the first one
	of the UReciprocalLatticeComponent type
	*/
	for (TObjectIterator<UReciprocalLatticeComponent> ObjectItr; ObjectItr; ++ObjectItr)
	{
		m_ReciprocalLattice = *ObjectItr;
	}

	if (!m_IsReciprocalBeam)
	{
		m_LocationOrigin = m_ReciprocalLattice->GetWorldLocation();
	}
	else
	{
		m_LocationOrigin = m_ReciprocalLattice->GetReciprocalWorldLocation();
	}

	// Sets the Z (up) component of the m_LocationOrigin to be the top of the Real-space lattice
	m_LocationOrigin.Z += m_ReciprocalLattice->GetDistanceBetweenNodesZ() * (m_ReciprocalLattice->GetNumberOfNodesZ() - 1);

	GetOwner()->SetActorLocation(FVector(m_LocationOrigin.X, m_LocationOrigin.Y, m_LocationOrigin.Z + m_DistanceFromLattice));
	GetOwner()->SetActorRotation(FQuat(FRotator(0.0f, 0.0f, 0.0f)));

	// Spawns particle system using m_ParticleSystemPrefab and the AActor's Location
	m_ParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_ParticleSystemPrefab, FTransform(GetOwner()->GetActorLocation()), false);
	// Sets the 3D size of the m_ParticleSystem
	m_ParticleSystem->SetWorldScale3D(FVector(m_Wavelength, m_Wavelength, m_Wavelength));
	// Sets the MaxCollisions of the m_ParticleSystem to be 1 initially
	m_ParticleSystem->SetFloatParameter("MaxCollisions", 1.0f);

	m_PlayerController = GetWorld()->GetFirstPlayerController();

	m_RadialMovementValue = 0.0f;
	m_RotationValue = 0.0f;
	m_ChangeInAngle = 0.0f;

	// Set all of Location variables to be the current location of the actor
	m_CurrentLocation = GetOwner()->GetActorLocation();
	m_LastLocation = m_CurrentLocation;
}

/**
Moves this AActor around the m_LocationOrigin by @movementSpeed amount
@param movementSpeed (float)
*/
void UXRayComponent::MoveRadially(float movementAmount)
{
	m_RadialMovementValue += movementAmount;
	GetOwner()->SetActorLocation(FVector(FMath::Sin(m_RadialMovementValue) * m_DistanceFromLattice, 0.0f, (FMath::Cos(m_RadialMovementValue) * m_DistanceFromLattice) + m_LocationOrigin.Z));

	m_CurrentLocation = GetOwner()->GetActorLocation();

	FindAngleBetweenLocations(m_CurrentLocation, m_LastLocation);
	FindTotalAngleFromOrigin(m_CurrentLocation);

	m_LastLocation = m_CurrentLocation;
}

/**
Rotates this AActor around its own X axis by @rotationAmount
This will be used to set the rotation of the m_ParticleSystem
@param rotationAmount (float)
*/
void UXRayComponent::ChangeParticleSystemRotation(float rotationAmount)
{
	m_RotationValue += -rotationAmount;
	GetOwner()->SetActorRotation(FQuat(FRotator(m_RotationValue, GetOwner()->GetActorRotation().Yaw, GetOwner()->GetActorRotation().Roll)));
}

/**
Adds debugging text to the screen using @text for the wSords
and @duration for the amount of time it will last on the screen in seconds
@param text (FString)
@param duration (float)
*/
void UXRayComponent::AddDebugTextToScreen(FString text, float duration)
{
	if (GEngine != NULL)
	{
		GEngine->AddOnScreenDebugMessage(0, duration, FColor::Yellow, text);
	}
}

/**
Calculates the m_ChangeInAngle using the @currentLocation and @lastLocation of the m_ParticleSystem
in respect to the m_LocationOrigin every frame
@param currentLocation (float)
@param lastLocation (float)
*/
void UXRayComponent::FindAngleBetweenLocations(FVector& currentLocation, FVector& lastLocation)
{
	FVector lastVector = FVector(lastLocation - m_LocationOrigin);
	FVector currentVector = FVector(currentLocation - m_LocationOrigin);

	m_ChangeInAngle = GetAngleBetweenVectors(lastVector, currentVector);
}

/**
Calculates the total m_Angle of the m_ParticleSystem using its @currentLocation
in respect to the m_ReciprocalLattice's ParallelOriginVector (the X Axis of the Lattice surface)
@param currentLocation (float)
*/
void UXRayComponent::FindTotalAngleFromOrigin(FVector & currentLocation)
{
	FVector currentVector = FVector(currentLocation - m_LocationOrigin);

	m_Angle = GetAngleBetweenVectors(m_ReciprocalLattice->ParallelOriginVector, currentVector);
}

/**
Returns: float
Returns the Dot Product of @v1 and @v2
@param v1 (FVector)
@param v2 (FVector)
*/
float UXRayComponent::DotProduct(FVector & v1, FVector & v2)
{
	return ((v1.X * v2.X) + (v1.Y * v2.Y) + (v1.Z * v2.Z));
}

/**
Returns: float
Calculates the Dot Product of @v1 and @v2 to return the angle between @v1 and @v2
@param v1 (FVector)
@param v2 (FVector)
*/
float UXRayComponent::GetAngleBetweenVectors(FVector & v1, FVector & v2)
{
	return FMath::Acos(((v1.X * v2.X) + (v1.Y * v2.Y) + (v1.Z * v2.Z)) / (v1.Size() * v2.Size()));
}

/**
Returns: FVector
Returns the resulting vector from subtracting @head - @tail
@param head (FVector)
@param tail (FVector)
*/
FVector UXRayComponent::GetVectorFromTwoPoints(FVector & head, FVector & tail)
{
	return FVector(head - tail);
}

/**
Manages the use of inputs from the Keyboard for changing the m_Wavelength and m_Angle at runtime
May be used for more in the future
The @deltaTime should come from the DeltaTime in the TickComponent() parameters
@param deltaTime (float)
*/
void UXRayComponent::ManageInputs(float & deltaTime)
{
	if (FMath::Sin(m_Angle) <= 1.0f && FMath::Sin(m_Angle) >= 0.0f)
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
	}

	if (m_Wavelength > 1.0f && m_Wavelength < 7.0f)
	{
		if (m_PlayerController->IsInputKeyDown(EKeys::L))
		{
			m_Wavelength += deltaTime;
		}
		else if (m_PlayerController->IsInputKeyDown(EKeys::J))
		{
			m_Wavelength -= deltaTime;
		}
	}
}

/**
~Still in progress~
Checks the Bragg Theorem conditions needed for the simulation to work so far
------------------------------------------------------------------------------------------
The original condition is:	lambda =	2 * d * sin(theta)
	where: lambda is m_Wavelength, d is m_ReciprocalLattice's GetDistanceBetweenNodesZ(),
	and theta is @theta the angle at which the particle beam is hitting the surface of the
	original lattice
------------------------------------------------------------------------------------------
The reciprocal condition is:	H* (1/d) =	(S - S0) / lambda
	where: lambda is m_Wavelength, d is m_ReciprocalLattice's GetDistanceBetweenNodesZ(),
	S0 is the incoming unit vector of the m_ParticleSystem's beam, S is the resultant unit vector
	of the m_ParticleSystem's beam, and H* is (1 / d)
@param theta (float)
*/
void UXRayComponent::ManageBraggTheorem(float theta)
{
	if (!m_IsReciprocalBeam)
	{
		m_TwoDSinTheta_Rounded = RoundToTenth(2.0f * m_ReciprocalLattice->GetDistanceBetweenNodesZ() * FMath::Sin(theta));
		m_Wavelength_Rounded = RoundToTenth(m_Wavelength);
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
	else
	{
		/* If this is a Reciprocal Beam, it needs to meet these conditions:
		S0 is the initial Unit Vector of the Beam coming in
		- You must get the original vector of S0 and convert it into a Unit Vector for calculation
		S is the output Unit Vector of the Beam coming out
		- You must get the original vector of S and convert it into a Unit Vector for calculation
		H* is the value of 1/d, Where d is the distance between the nodes of the Reciprocal Lattice

		This is the formula:

		H* = S - S0
		------
		lambda

		When this condition is met, a beam will come out similar to the Real Space Lattice Beam
		Make sure to put a vertical beam which is the addition of the Initial and Resultant Beams
		*/

		FVector CurrentLocation = GetOwner()->GetActorLocation();
		FVector IncomingVector = GetVectorFromTwoPoints(m_LocationOrigin, CurrentLocation);
		FVector IncomingUnitVector = IncomingVector / IncomingVector.Size();
		bool IncomingVectorIsUnitVector = RoundToTenth(IncomingUnitVector.Size()) == 1.0f;

		if (IncomingVectorIsUnitVector)
		{
			AddDebugTextToScreen("It Is a Unit Vector!!!", 1.5f);

			float Dot = DotProduct(IncomingVector, m_ReciprocalLattice->ParallelOriginVector);

			/*
				DotProduct = ||v1|| ||v2|| cos(theta)
				||v2|| =		DotProduct
							-----------------
							||v1|| cos(theta)
			*/
			float ResultantVectorMagnitude = Dot / (IncomingUnitVector.Size() * FMath::Cos(-1.0f * GetAngleBetweenVectors(IncomingVector, m_ReciprocalLattice->ParallelOriginVector)));

			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.5f, FColor::Yellow, "S - S0: " + FString::SanitizeFloat(RoundToTenth(ResultantVectorMagnitude - IncomingVector.Size())));

			// The Right side of the formula
			float ReciprocalFormula = (IncomingVector.Size() - ResultantVectorMagnitude) / m_Wavelength;

			if (RoundToTenth(ReciprocalFormula) == RoundToTenth(1.0f / m_ReciprocalLattice->GetDistanceBetweenReciprocalNodesZ()))
			{
				m_ParticleSystem->SetFloatParameter("MaxCollisions", 2.0f);
			}
			else
			{
				m_ParticleSystem->SetFloatParameter("MaxCollisions", 1.0f);
			}
		}
		else
		{
			AddDebugTextToScreen("Value of Incoming Unit Vector" + FString::SanitizeFloat(RoundToTenth(IncomingVector.Size())), 1.5f);
		}
	}
}

// Called every frame
void UXRayComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// This has to be done in order for the m_ReciprocalLattice's components to be initialized
	if (UGameplayStatics::GetRealTimeSeconds(this) > 2.0f)
	{
		if (!m_IsXRayComponentInitialized)
		{
			InitializeXRayComponents();

			m_IsXRayComponentInitialized = true;
		}
		else
		{
			ManageInputs(DeltaTime);
			ManageBraggTheorem(FMath::Abs(m_Angle));

			// Sets the 3D size of the m_ParticleSystem
			m_ParticleSystem->SetWorldScale3D(FVector(m_Wavelength, m_Wavelength, m_Wavelength));
			//
			m_ParticleSystem->SetWorldRotation(FQuat(GetOwner()->GetActorRotation()));
			m_ParticleSystem->SetWorldLocation(GetOwner()->GetActorLocation());
		}
	}

	// ...
}

/**
Returns: float
Used as a quick way to round a floating point variable to one decimal place
@param valueToRound (float)
*/
float UXRayComponent::RoundToTenth(float valueToRound)
{
	return round(valueToRound * 10.0f) / 10.0f;
}
