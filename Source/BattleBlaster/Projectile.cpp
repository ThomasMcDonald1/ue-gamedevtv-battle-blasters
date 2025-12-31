// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root Comp"));
	SetRootComponent(ProjectileMesh);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComponent->InitialSpeed = 1000.0f;
	ProjectileMovementComponent->MaxSpeed = 1000.0f;

	TrailParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailParticles"));
	TrailParticles->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), LaunchSound, GetActorLocation());
	}
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (AActor* MyOwner = GetOwner())
	{
		if (OtherActor && OtherActor != MyOwner && OtherActor != this)
		{
			// Wouldn't we want to make a conditional check for if OtherActor is a valid actor to take damage? I don't think we want to be calling this every time
			// we hit a wall or some other kind of collision... right?

			UGameplayStatics::ApplyDamage(OtherActor, this->Damage, MyOwner->GetInstigatorController(),
				this, UDamageType::StaticClass());

			if (HitParticles)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitParticles, GetActorLocation(), GetActorRotation());
			}

			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
			}

			if (HitCameraShakeClass)
			{
				if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
				{
					PlayerController->ClientStartCameraShake(HitCameraShakeClass);
				}
			}
			//if (OtherActor)
			//{
			//	UE_LOG(LogTemp, Display, TEXT("Actor hit: %s"), *OtherActor->GetActorNameOrLabel());
			//}
		}
	}

	Destroy();
}

