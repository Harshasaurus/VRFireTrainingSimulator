#include "VRGrabbable.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

AVRGrabbable::AVRGrabbable()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create the mesh ? this is the visible object
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    // Enable physics so it falls and reacts to throws
    MeshComponent->SetSimulatePhysics(true);
    MeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));

    // Create grab sphere ? hand must overlap this to grab
    GrabSphere = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphere"));
    GrabSphere->SetupAttachment(RootComponent);
    GrabSphere->SetSphereRadius(15.0f);
    GrabSphere->SetCollisionProfileName(TEXT("OverlapAll"));

    bIsGrabbed = false;
}

void AVRGrabbable::BeginPlay()
{
    Super::BeginPlay();
}

void AVRGrabbable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AVRGrabbable::Grab(USceneComponent* AttachTo)
{
    if (!AttachTo) return;

    bIsGrabbed = true;

    // Disable physics while held ? object follows hand
    MeshComponent->SetSimulatePhysics(false);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Attach to the hand component
    AttachToComponent(AttachTo,
        FAttachmentTransformRules(
            EAttachmentRule::SnapToTarget,  // snap position to hand
            EAttachmentRule::SnapToTarget,  // snap rotation to hand
            EAttachmentRule::KeepWorld,     // keep world scale
            true                            // weld bodies
        )
    );
}

void AVRGrabbable::Release(FVector ThrowVelocity)
{
    bIsGrabbed = false;

    // Detach from hand
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    // Re-enable physics so object flies through air
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComponent->SetSimulatePhysics(true);

    // Apply throw velocity ? this makes it feel like real throwing
    MeshComponent->SetPhysicsLinearVelocity(ThrowVelocity);
}