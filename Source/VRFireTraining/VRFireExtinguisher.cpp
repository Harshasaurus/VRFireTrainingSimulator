#include "VRFireExtinguisher.h"
#include "VRFire.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"

AVRFireExtinguisher::AVRFireExtinguisher()
{
    PrimaryActorTick.bCanEverTick = true;

    // Main mesh
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(
        TEXT("MeshComponent"));
    RootComponent = MeshComponent;
    MeshComponent->SetSimulatePhysics(true);
    MeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));

    // Grab sphere
    GrabSphere = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphere"));
    GrabSphere->SetupAttachment(RootComponent);
    GrabSphere->SetSphereRadius(15.0f);
    GrabSphere->SetCollisionProfileName(TEXT("OverlapAll"));

    // Spray particle
    SprayParticle = CreateDefaultSubobject<UParticleSystemComponent>(
        TEXT("SprayParticle"));
    SprayParticle->SetupAttachment(RootComponent);
    SprayParticle->SetAutoActivate(false);
}

void AVRFireExtinguisher::BeginPlay()
{
    Super::BeginPlay();
}

void AVRFireExtinguisher::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsSpraying)
    {
        SprayTick(DeltaTime);
    }
}

void AVRFireExtinguisher::Grab(USceneComponent* AttachTo)
{
    if (!AttachTo) return;

    bIsGrabbed = true;
    MeshComponent->SetSimulatePhysics(false);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    AttachToComponent(AttachTo,
        FAttachmentTransformRules(
            EAttachmentRule::SnapToTarget,
            EAttachmentRule::SnapToTarget,
            EAttachmentRule::KeepWorld,
            true
        )
    );

    UE_LOG(LogTemp, Warning, TEXT("Extinguisher grabbed!"));
}

void AVRFireExtinguisher::Release(FVector ThrowVelocity)
{
    bIsGrabbed = false;
    bIsSpraying = false;
    SprayParticle->Deactivate();

    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComponent->SetSimulatePhysics(true);
    MeshComponent->SetPhysicsLinearVelocity(ThrowVelocity);

    UE_LOG(LogTemp, Warning, TEXT("Extinguisher released!"));
}

void AVRFireExtinguisher::StartSpray()
{
    if (!bIsGrabbed) return;

    bIsSpraying = true;
    SprayParticle->Activate();

    UE_LOG(LogTemp, Warning, TEXT("Spraying!"));
}

void AVRFireExtinguisher::StopSpray()
{
    bIsSpraying = false;
    SprayParticle->Deactivate();

    UE_LOG(LogTemp, Warning, TEXT("Stopped spraying!"));
}

void AVRFireExtinguisher::SprayTick(float DeltaTime)
{
    // Raycast forward from extinguisher nozzle
    FVector Start = GetActorLocation();
    FVector Forward = GetActorForwardVector();
    FVector End = Start + (Forward * SprayRange);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult, Start, End,
        ECC_Visibility, Params
    );

    // Debug spray line
    DrawDebugLine(GetWorld(), Start, End,
        FColor::Cyan, false, -1.0f, 0, 1.0f);

    if (bHit)
    {
        AVRFire* Fire = Cast<AVRFire>(HitResult.GetActor());
        if (Fire)
        {
            Fire->ApplyExtinguisher(DeltaTime);
        }
    }
}