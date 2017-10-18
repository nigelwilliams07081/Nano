// Fill out your copyright notice in the Description page of Project Settings.

#include "ReciprocalLatticeComponent.h"
#include "ConstructorHelpers.h"


// Sets default values for this component's properties
UReciprocalLatticeComponent::UReciprocalLatticeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UBlueprint> LatticeSphereBlueprintObject(TEXT("Blueprint'/Game/BluePrints/LatticeSphere.LatticeSphere'"));
	m_SpherePrefab = (UClass*)LatticeSphereBlueprintObject.Object->GeneratedClass;

	static ConstructorHelpers::FObjectFinder<UBlueprint> ReciprocalLatticeSphereBlueprintObject(TEXT("Blueprint'/Game/BluePrints/ReciprocalLatticeSphere.ReciprocalLatticeSphere'"));
	m_ReciprocalSpherePrefab = (UClass*)ReciprocalLatticeSphereBlueprintObject.Object->GeneratedClass;
	// ...
}


// Called when the game starts
void UReciprocalLatticeComponent::BeginPlay()
{
	Super::BeginPlay();

	CreateCubicLattice(m_NumberOfSpheresPerVector, m_LatticeLengthX, m_LatticeLengthY, m_LatticeLengthZ, m_SphereSize);
	SetReciprocalLatticeVectors(m_A1LatticeVector, m_A2LatticeVector, m_A3LatticeVector);
	CreateReciprocalCubicLattice(m_NumberOfSpheresPerReciprocalVector, m_B1ReciprocalLatticeVector, m_B2ReciprocalLatticeVector, m_B3ReciprocalLatticeVector, m_ReciprocalSphereSize);
	// ...
	
}


// Called every frame
void UReciprocalLatticeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UReciprocalLatticeComponent::CreateCubicLattice(int& numberOfSpheres, float& lengthX, float& lengthY, float& lengthZ, float& sphereSize)
{
	float spaceBetweenSpheresX = (m_LatticeLengthX * CONVERSION_FACTOR) / (numberOfSpheres - 1);
	float spaceBetweenSpheresY = (m_LatticeLengthY * CONVERSION_FACTOR) / (numberOfSpheres - 1);
	float spaceBetweenSpheresZ = (m_LatticeLengthZ * CONVERSION_FACTOR) / (numberOfSpheres - 1);

	for (int i = 0; i < numberOfSpheres; i++)
	{
		for (int j = 0; j < numberOfSpheres; j++)
		{
			for (int k = 0; k < numberOfSpheres; k++)
			{
				AActor* Sphere = GetWorld()->SpawnActor<AActor>(m_SpherePrefab);
				Sphere->SetActorLocation(FVector(m_WorldLocation.X + (k * spaceBetweenSpheresX), m_WorldLocation.Y + (j * spaceBetweenSpheresY), m_WorldLocation.Z + (i * spaceBetweenSpheresZ)));
				Sphere->SetActorScale3D(FVector(sphereSize, sphereSize, sphereSize));
			}
		}
	}

	SetLatticeVectors(m_LatticeLengthX, m_LatticeLengthY, m_LatticeLengthZ);
}

void UReciprocalLatticeComponent::CreateReciprocalCubicLattice(int& numberOfSpheres, FVector& b1, FVector& b2, FVector& b3, float& sphereSize)
{
	FVector G = b1 + b2 + b3;

	float spaceBetweenSpheresX = G.X / (numberOfSpheres - 1);
	float spaceBetweenSpheresY = G.Y / (numberOfSpheres - 1);
	float spaceBetweenSpheresZ = G.Z / (numberOfSpheres - 1);

	for (int i = 0; i < numberOfSpheres; i++)
	{
		for (int j = 0; j < numberOfSpheres; j++)
		{
			for (int k = 0; k < numberOfSpheres; k++)
			{
				AActor* Sphere = GetWorld()->SpawnActor<AActor>(m_ReciprocalSpherePrefab);
				Sphere->SetActorLocation(FVector(m_ReciprocalWorldLocation.X + (k * spaceBetweenSpheresX), m_ReciprocalWorldLocation.Y + (j * spaceBetweenSpheresY), m_ReciprocalWorldLocation.Z + (i * spaceBetweenSpheresZ)));
				Sphere->SetActorScale3D(FVector(sphereSize, sphereSize, sphereSize));
			}
		}
	}
}

void UReciprocalLatticeComponent::SetLatticeVectors(float& lengthX, float& lengthY, float& lengthZ)
{
	m_A1LatticeVector = FVector(lengthX, (float)CONVERSION_FACTOR, (float)CONVERSION_FACTOR);
	m_A2LatticeVector = FVector((float)CONVERSION_FACTOR, lengthY, (float)CONVERSION_FACTOR);
	m_A3LatticeVector = FVector((float)CONVERSION_FACTOR, (float)CONVERSION_FACTOR, lengthZ);
}

void UReciprocalLatticeComponent::SetReciprocalLatticeVectors(FVector& a1, FVector& a2, FVector& a3)
{
	m_CrossProductVector = CrossProduct(a2, a3, AngleBetweenVectors(a2, a3));
	m_B1ReciprocalLatticeVector = (m_CrossProductVector / (a1 * m_CrossProductVector));

	m_CrossProductVector = CrossProduct(a3, a1, AngleBetweenVectors(a3, a1));
	m_B2ReciprocalLatticeVector = (m_CrossProductVector / (a2 * m_CrossProductVector));

	m_CrossProductVector = CrossProduct(a1, a2, AngleBetweenVectors(a1, a2));
	m_B3ReciprocalLatticeVector = (m_CrossProductVector / (a3 * m_CrossProductVector));
}

FVector UReciprocalLatticeComponent::CrossProduct(FVector& v1, FVector& v2, float angleBetweenVectors)
{
	return v1 * v2 * FMath::Asin(angleBetweenVectors);
}

// Returns the angle between the Vectors in Radians
float UReciprocalLatticeComponent::AngleBetweenVectors(FVector& v1, FVector& v2)
{
	float dotProduct = FVector::DotProduct(v1, v2);
	return FMath::Acos(dotProduct / (v1.Size() * v2.Size()));
}