// Fill out your copyright notice in the Description page of Project Settings.

#include "ReciprocalLatticeComponent.h"
#include "ConstructorHelpers.h"
#include "Engine.h"


// Sets default values for this component's properties
UReciprocalLatticeComponent::UReciprocalLatticeComponent() : m_DistanceBetweenNodesZ(0.0f), m_SpherePrefab(nullptr), m_ReciprocalSpherePrefab(nullptr), m_PlanePrefab(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Holds the Blueprint Object made in the Editor
	static ConstructorHelpers::FObjectFinder<UClass> LatticeSphereBlueprintObject(TEXT("Class'/Game/BluePrints/LatticeSphere.LatticeSphere_C'"));

	// if the Blueprint Object is not nullptr, Instantiate it, otherwise Print an Error Message
	LatticeSphereBlueprintObject.Object ?
		m_SpherePrefab = (UClass*)LatticeSphereBlueprintObject.Object :
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.5f, FColor::Yellow, "Sphere Blueprint is Nullptr");

	// Holds the Blueprint Object made in the Editor
	static ConstructorHelpers::FObjectFinder<UClass> ReciprocalLatticeSphereBlueprintObject(TEXT("Class'/Game/BluePrints/ReciprocalLatticeSphere.ReciprocalLatticeSphere_C'"));

	// if the Blueprint Object is not nullptr, Instantiate it, otherwise Print an Error Message
	ReciprocalLatticeSphereBlueprintObject.Object ?
		m_ReciprocalSpherePrefab = (UClass*)ReciprocalLatticeSphereBlueprintObject.Object :
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.5f, FColor::Yellow, "Reciprocal Sphere Blueprint is Nullptr");

	// Holds the Blueprint Object made in the Editor
	static ConstructorHelpers::FObjectFinder<UClass> LatticePlaneBlueprintObject(TEXT("Class'/Game/Blueprints/LatticePlane.LatticePlane_C'"));

	// if the Blueprint Object is not nullptr, Instantiate it, otherwise Print an Error Message
	LatticePlaneBlueprintObject.Object ?
		m_PlanePrefab = (UClass*)LatticePlaneBlueprintObject.Object :
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.5f, FColor::Yellow, "Plane Blueprint is Nullptr");

	// ...
}


// Called when the game starts
void UReciprocalLatticeComponent::BeginPlay()
{
	Super::BeginPlay();

	if (m_SpherePrefab != nullptr && m_ReciprocalSpherePrefab != nullptr && m_PlanePrefab != nullptr)
	{
		// Sets the paralle vector for the UXRayComponent to access
		ParallelOriginVector = FVector(1.0f, 0.0f, 0.0f);

		CreateCubicLattice(m_NumberOfSpheresPerVector, m_LatticeLengthX, m_LatticeLengthY, m_LatticeLengthZ, m_SphereSize);

		SetReciprocalLatticeVectors(m_A1LatticeVector, m_A2LatticeVector, m_A3LatticeVector);

		CreateReciprocalCubicLattice(m_NumberOfSpheresPerReciprocalVector, m_B1ReciprocalLatticeVector, m_B2ReciprocalLatticeVector, m_B3ReciprocalLatticeVector, m_ReciprocalSphereSize);
	}
	// ...

}


// Called every frame
void UReciprocalLatticeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

/**
Creates a 3D cubic lattice with the specified number of spheres,
the lengths of each dimension of the lattice, and the size of the spheres
It also creates a Plane on top of each x,y plane of the lattice
@param numberOfSpheres (int)
@param lengthX (float)
@param lengthY (float)
@param lengthZ (float)
@param sphereSize (float)
*/
void UReciprocalLatticeComponent::CreateCubicLattice(int& numberOfSpheres, float& lengthX, float& lengthY, float& lengthZ, float& sphereSize)
{
	float spaceBetweenSpheresX = (lengthX * CONVERSION_FACTOR) / (numberOfSpheres - 1);
	float spaceBetweenSpheresY = (lengthY * CONVERSION_FACTOR) / (numberOfSpheres - 1);
	float spaceBetweenSpheresZ = (lengthZ * CONVERSION_FACTOR) / (numberOfSpheres - 1);
	m_DistanceBetweenNodesZ = spaceBetweenSpheresZ;
	m_NumberOfNodesZ = numberOfSpheres;

	for (int i = 0; i < numberOfSpheres; i++)
	{
		for (int j = 0; j < numberOfSpheres; j++)
		{
			for (int k = 0; k < numberOfSpheres; k++)
			{
				// Spawns an AActor sphere using the m_SpherePrefab

				AActor* Sphere = GetWorld()->SpawnActor<AActor>(m_SpherePrefab);

				Sphere->SetActorLocation(FVector(m_WorldLocation.X + (k * spaceBetweenSpheresX), m_WorldLocation.Y + (j * spaceBetweenSpheresY), m_WorldLocation.Z + (i * spaceBetweenSpheresZ)));
				Sphere->SetActorScale3D(FVector(sphereSize, sphereSize, sphereSize));

				if (k == numberOfSpheres - 1)
				{
					CreatePlane(FVector(m_WorldLocation.X + (lengthX * CONVERSION_FACTOR / 2.0f), m_WorldLocation.Y + (lengthY * CONVERSION_FACTOR / 2.0f), m_WorldLocation.Z + (i * spaceBetweenSpheresZ + sphereSize * CONVERSION_FACTOR * 0.5f)), lengthX, lengthY);
				}
			}
		}
	}

	SetLatticeVectors(lengthX, lengthY, lengthZ);
}

/**
Creates a 3D cubic reciprocal lattice with the specified number of spheres,
the lengths of each dimension of the reciprocal lattice, and the size of the spheres
It WILL also create a Plane on top of each x,y plane of the lattice ~Incomplete~
@param numberOfSpheres (int)
@param b1 (FVector)
@param b2 (FVector)
@param b3 (FVector)
@param sphereSize (float)
*/
void UReciprocalLatticeComponent::CreateReciprocalCubicLattice(int& numberOfSpheres, FVector& b1, FVector& b2, FVector& b3, float& sphereSize)
{
	FVector G = b1 + b2 + b3;

	float spaceBetweenSpheresX = G.X / (numberOfSpheres - 1);
	float spaceBetweenSpheresY = G.Y / (numberOfSpheres - 1);
	float spaceBetweenSpheresZ = G.Z / (numberOfSpheres - 1);

	float lengthX = G.X / (float)CONVERSION_FACTOR;
	float lengthY = G.Y / (float)CONVERSION_FACTOR;
	float lengthZ = G.Z / (float)CONVERSION_FACTOR;

	m_DistanceBetweenReciprocalNodesZ = spaceBetweenSpheresZ;
	m_NumberOfReciprocalNodesZ = numberOfSpheres;

	for (int i = 0; i < numberOfSpheres; i++)
	{
		for (int j = 0; j < numberOfSpheres; j++)
		{
			for (int k = 0; k < numberOfSpheres; k++)
			{
				AActor* Sphere = GetWorld()->SpawnActor<AActor>(m_ReciprocalSpherePrefab);
				Sphere->SetActorLocation(FVector(m_ReciprocalWorldLocation.X + (k * spaceBetweenSpheresX), m_ReciprocalWorldLocation.Y + (j * spaceBetweenSpheresY), m_ReciprocalWorldLocation.Z + (i * spaceBetweenSpheresZ)));
				Sphere->SetActorScale3D(FVector(sphereSize, sphereSize, sphereSize));

				if (k == numberOfSpheres - 1)
				{
					CreatePlane(FVector(m_ReciprocalWorldLocation.X + (G.X / 2.0f), m_ReciprocalWorldLocation.Y + (G.Y / 2.0f), m_ReciprocalWorldLocation.Z + (i * spaceBetweenSpheresZ + sphereSize * CONVERSION_FACTOR * 0.5f)), lengthX, lengthY);
				}
			}
		}
	}
}

/**
Sets the length of each Lattice Vector for use later
@param lengthX (float)
@param lengthY (float)
@param lengthZ (float)
*/
void UReciprocalLatticeComponent::SetLatticeVectors(float& lengthX, float& lengthY, float& lengthZ)
{
	m_A1LatticeVector = FVector(lengthX, (float)CONVERSION_FACTOR, (float)CONVERSION_FACTOR);
	m_A2LatticeVector = FVector((float)CONVERSION_FACTOR, lengthY, (float)CONVERSION_FACTOR);
	m_A3LatticeVector = FVector((float)CONVERSION_FACTOR, (float)CONVERSION_FACTOR, lengthZ);
}

/**
Sets the Reciprocal Lattice vectors by the cross product of combinations two of the three given vectors
and the angle between them
@param a1 (FVector)
@param a2 (FVector)
@param a3 (FVector)
*/
void UReciprocalLatticeComponent::SetReciprocalLatticeVectors(FVector& a1, FVector& a2, FVector& a3)
{
	m_CrossProductVector = CrossProduct(a2, a3, AngleBetweenVectors(a2, a3));
	m_B1ReciprocalLatticeVector = (m_CrossProductVector / (a1 * m_CrossProductVector));

	m_CrossProductVector = CrossProduct(a3, a1, AngleBetweenVectors(a3, a1));
	m_B2ReciprocalLatticeVector = (m_CrossProductVector / (a2 * m_CrossProductVector));

	m_CrossProductVector = CrossProduct(a1, a2, AngleBetweenVectors(a1, a2));
	m_B3ReciprocalLatticeVector = (m_CrossProductVector / (a3 * m_CrossProductVector));
}

/**
Creates a Plane at the given @location of size (@lengthX, @lengthY)
@param location (FVector)
@param lengthX (float)
@param lengthY (float)
*/
void UReciprocalLatticeComponent::CreatePlane(FVector location, float & lengthX, float & lengthY)
{
	AActor* Plane = GetWorld()->SpawnActor(m_PlanePrefab);
	Plane->SetActorLocation(location);
	Plane->SetActorScale3D(FVector(lengthX, lengthY, 1.0f));
}

/**
Returns: FVector
Returns the cross product of @v1 and @v2 using the @angleBetweenVectors
@param v1 (FVector)
@param v2 (FVector)
@param
*/
FVector UReciprocalLatticeComponent::CrossProduct(FVector& v1, FVector& v2, float angleBetweenVectors)
{
	return v1 * v2 * FMath::Asin(angleBetweenVectors);
}

/**
Returns: float
Returns the angle between @v1 and @v2
@param v1 (FVector)
@param v2 (FVector)
*/
float UReciprocalLatticeComponent::AngleBetweenVectors(FVector& v1, FVector& v2)
{
	float dotProduct = FVector::DotProduct(v1, v2);
	return FMath::Acos(dotProduct / (v1.Size() * v2.Size()));
}

float UReciprocalLatticeComponent::GetDistanceBetweenNodesZ() const
{
	return m_DistanceBetweenNodesZ;
}

int UReciprocalLatticeComponent::GetNumberOfNodesZ() const
{
	return m_NumberOfNodesZ;
}

float UReciprocalLatticeComponent::GetDistanceBetweenReciprocalNodesZ() const
{
	return m_DistanceBetweenReciprocalNodesZ;
}

int UReciprocalLatticeComponent::GetNumberOfReciprocalNodesZ() const
{
	return m_NumberOfReciprocalNodesZ;
}

FVector UReciprocalLatticeComponent::GetWorldLocation() const
{
	return m_WorldLocation;
}

FVector UReciprocalLatticeComponent::GetReciprocalWorldLocation() const
{
	return m_ReciprocalWorldLocation;
}

