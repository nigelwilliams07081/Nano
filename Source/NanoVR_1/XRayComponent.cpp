// Fill out your copyright notice in the Description page of Project Settings.

#include "XRayComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"


// Sets default values for this component's properties
UXRayComponent::UXRayComponent()
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
	// Find the UReciprocalLatticeComponent from the Scene
	//m_Lattice = GetWorld();
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_ParticleSystemList.Add(UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_ParticleSystemPrefab, FTransform(FVector(m_WorldLocation.X + 10.0f * i, m_WorldLocation.Y + 10.0f * j, m_WorldLocation.Z)), false));
		}
	}

	/*for (int i = 0; i < m_Lattice->GetSurfaceNodePoints().Num(); i++)
	{
		m_ParticleSystemList[i]->SetBeamTargetPoint(0, m_Lattice->GetSurfaceNodePoints()[i], 0);
	}*/
	
	m_PlayerController = GetWorld()->GetFirstPlayerController();
	m_ParticleSystemVelocity = FVector(0.0f, 0.0f, -100.0f);
	// ...
	
}

void UXRayComponent::RotateVertical(float rotationAmount)
{
	m_ParticleSystemVelocity.X += rotationAmount;
}

void UXRayComponent::AddDebugTextToScreen(FString text)
{
	if (GEngine != NULL)
	{
		GEngine->AddOnScreenDebugMessage(0, 0.5f, FColor::Yellow, text);
	}
}


// Called every frame
void UXRayComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_PlayerController->IsInputKeyDown(EKeys::I))
	{
		RotateVertical(m_VerticalRotationSpeed * DeltaTime);
	}
	else if (m_PlayerController->IsInputKeyDown(EKeys::K))
	{
		RotateVertical(-m_VerticalRotationSpeed * DeltaTime);
	}

	for (int i = 0; i < m_ParticleSystemList.Num(); i++)
	{
		m_ParticleSystemList[i]->SetWorldRotation(FQuat(FRotator(m_ParticleSystemVelocity.X, 0.0f, 0.0f)));
	}
	//m_ParticleSystem->SetWorldRotation(FQuat(FRotator(m_ParticleSystemVelocity.X, 0.0f, 0.0f)));
	// ...
}

//UParticleSystemComponent * UXRayComponent::GetParticleSystem() const
//{
//	return m_ParticleSystem;
//}

