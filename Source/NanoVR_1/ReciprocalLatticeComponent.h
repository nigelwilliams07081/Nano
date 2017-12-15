// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReciprocalLatticeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NANOVR_1_API UReciprocalLatticeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UReciprocalLatticeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void CreateCubicLattice(int& numberOfSpheres, float& lengthX, float& lengthY, float& lengthZ, float& sphereSize);
	void CreateReciprocalCubicLattice(int& numberOfSpheres, FVector& b1, FVector& b2, FVector& b3, float& sphereSize);
	void SetLatticeVectors(float& lengthX, float& lengthY, float& lengthZ);
	void SetReciprocalLatticeVectors(FVector& a1, FVector& a2, FVector& a3);

	void CreatePlane(FVector location, float& lengthX, float& lengthY);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	Returns: FVector
	Returns the Cross product of @v1 and @v2, utilizing the @angleBetweenVectors
	@param v1 (FVector)
	@param v2 (FVector)
	*/
	FVector CrossProduct(FVector& v1, FVector& v2, float angleBetweenVectors);

	/**
	Returns: float
	Returns the angle between @v1 and @v2 using FVector::DotProduct() and FMath::Acos()
	@param v1 (FVector)
	@param v2 (FVector)
	*/
	float AngleBetweenVectors(FVector& v1, FVector& v2);

	/**
	Returns: float
	Returns m_DistanceBetweenNodesZ
	*/
	float GetDistanceBetweenNodesZ() const;

	/**
	Returns: int
	Returns m_NumberOfNodesZ
	*/
	int GetNumberOfNodesZ() const;

	/**
	Returns: float
	Returns m_DistanceBetweenReciprocalNodesZ
	*/
	float GetDistanceBetweenReciprocalNodesZ() const;

	/**
	Returns: int
	Returns m_NumberOfReciprocalNodesZ
	*/
	int GetNumberOfReciprocalNodesZ() const;

	/**
	Returns: FVector
	Returns the current World Location of the Real-space Lattice
	*/
	FVector GetWorldLocation() const;

	/**
	Returns: FVector
	Returns the current World Location of the Reciprocal-space Lattice
	*/
	FVector GetReciprocalWorldLocation() const;

	/**
	The vector used to represent the top surface of the real-space and reciprocal-space lattices
	*/
	FVector ParallelOriginVector;

private:

	UPROPERTY(EditAnywhere, DisplayName = "Lattice Length X")
		float m_LatticeLengthX;
	UPROPERTY(EditAnywhere, DisplayName = "Lattice Length Y")
		float m_LatticeLengthY;
	UPROPERTY(EditAnywhere, DisplayName = "Lattice Length Z")
		float m_LatticeLengthZ;
	UPROPERTY(EditAnywhere, DisplayName = "Number Of Spheres Per Vector", meta = (ClampMin = 0, ClampMax = 8))
		int m_NumberOfSpheresPerVector;
	UPROPERTY(EditAnywhere, DisplayName = "World Location")
		FVector m_WorldLocation;
	UPROPERTY(EditAnywhere, DisplayName = "Sphere Size")
		float m_SphereSize;

	UPROPERTY(EditAnywhere, DisplayName = "Number Of Spheres Per Reciprocal Vector", meta = (ClampMin = 0, ClampMax = 8))
		int m_NumberOfSpheresPerReciprocalVector;
	UPROPERTY(EditAnywhere, DisplayName = "Reciprocal Sphere Size")
		float m_ReciprocalSphereSize;
	UPROPERTY(EditAnywhere, DisplayName = "Reciprocal World Location (Testing)")
		FVector m_ReciprocalWorldLocation;

	// The prefab to be used for spawning a real-space sphere
	UClass* m_SpherePrefab;
	// The prefab to be used for spawning a reciprocal-space sphere
	UClass* m_ReciprocalSpherePrefab;
	// The prefab to be used for spawning a real-space plane
	UClass* m_PlanePrefab;
	// The prefab to be used for spawning a reciprocal-space plane
	UClass* m_ReciprocalPlanePrefab;

	FVector m_A1LatticeVector;
	FVector m_A2LatticeVector;
	FVector m_A3LatticeVector;
	FVector m_B1ReciprocalLatticeVector;
	FVector m_B2ReciprocalLatticeVector;
	FVector m_B3ReciprocalLatticeVector;

	FVector m_CrossProductVector;

	// Used to hold the distance between the nodes in the Z (up) direction
	float m_DistanceBetweenNodesZ;
	// Used to hold the number of nodes in the Z (up) direction
	int m_NumberOfNodesZ;

	// Used to hold the distance betwen the reciprocal nodes in the Z direction
	float m_DistanceBetweenReciprocalNodesZ;
	// Used to hold the number of nodes in the Z direction
	int m_NumberOfReciprocalNodesZ;

	// The conversion-factor used to translate the units from centimeters (default) to meters in the World
	const int CONVERSION_FACTOR = 100;
};
