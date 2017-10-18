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

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CreateCubicLattice(int& numberOfSpheres, float& lengthX, float& lengthY, float& lengthZ, float& sphereSize);
	void CreateReciprocalCubicLattice(int& numberOfSpheres, FVector& b1, FVector& b2, FVector& b3, float& sphereSize);
	void SetLatticeVectors(float& lengthX, float& lengthY, float& lengthZ);
	void SetReciprocalLatticeVectors(FVector& a1, FVector& a2, FVector& a3);

	FVector CrossProduct(FVector& v1, FVector& v2, float angleBetweenVectors);
	float AngleBetweenVectors(FVector& v1, FVector& v2);

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

	UClass* m_SpherePrefab;
	UClass* m_ReciprocalSpherePrefab;

	FVector m_A1LatticeVector;
	FVector m_A2LatticeVector;
	FVector m_A3LatticeVector;
	FVector m_B1ReciprocalLatticeVector;
	FVector m_B2ReciprocalLatticeVector;
	FVector m_B3ReciprocalLatticeVector;

	FVector m_CrossProductVector;

	const int CONVERSION_FACTOR = 100;
};
