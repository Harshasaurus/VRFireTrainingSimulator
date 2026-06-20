#include "VRFireExtinguisher.h"
#include "VRFire.h"
#include "VRInstructionSystem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"

AVRFireExtinguisher::AVRFireExtinguisher()
{
    PrimaryActorTick.bCanEverTick = true;

    SprayParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SprayParticle"));
    SprayParticle->SetupAttachment(RootComponent);
    SprayParticle->SetAutoActivate(false);
}

void AVRFireExtinguisher::BeginPlay()
{
    Super::BeginPlay();
}

void AVRFireExtinguisher::FindInstructionSystem()
{
    if (InstructionSystem) return;

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(),
        AVRInstructionSystem::StaticClass(), Found);
    if (Found.Num() > 0)
        InstructionSystem = Cast<AVRInstructionSystem>(Found[0]);

    if (!InstructionSystem)
        UE_LOG(LogTemp, Error, TEXT("InstructionSystem is NULL!"));
}

void AVRFireExtinguisher::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (bIsSpraying)
        SprayTick(DeltaTime);
}

void AVRFireExtinguisher::Grab(USceneComponent* AttachTo)
{
    Super::Grab(AttachTo);
    UE_LOG(LogTemp, Warning, TEXT("Extinguisher grabbed!"));

    if (!bHasGrabbedBefore)
    {
        bHasGrabbedBefore = true;
        FindInstructionSystem();
        if (InstructionSystem)
        {
            InstructionSystem->NextInstruction();
            UE_LOG(LogTemp, Warning, TEXT("NextInstruction called from Grab"));
        }
    }
}
void AVRFireExtinguisher::Release(FVector ThrowVelocity)
{
    bIsSpraying = false;
    bPinPulled = false;
    bHasStartedSprayBefore = false;
    bHasGrabbedBefore = false;
    SprayParticle->Deactivate();

    Super::Release(ThrowVelocity);
    UE_LOG(LogTemp, Warning, TEXT("Extinguisher released!"));

    FindInstructionSystem();
    if (InstructionSystem)
    {
        InstructionSystem->StartTraining();
        UE_LOG(LogTemp, Warning, TEXT("Training reset on release"));
    }
}
void AVRFireExtinguisher::PullPin()
{
    if (!bIsGrabbed || bPinPulled) return;

    bPinPulled = true;
    UE_LOG(LogTemp, Warning, TEXT("Pin pulled!"));

    FindInstructionSystem();
    if (InstructionSystem)
    {
        InstructionSystem->NextInstruction();
        UE_LOG(LogTemp, Warning, TEXT("NextInstruction called from PullPin"));
    }
}

void AVRFireExtinguisher::StartSpray()
{
    if (!bPinPulled) return;

    bIsSpraying = true;
    SprayParticle->Activate();
    UE_LOG(LogTemp, Warning, TEXT("Spraying!"));

    if (!bHasStartedSprayBefore)
    {
        bHasStartedSprayBefore = true;
        FindInstructionSystem();
        if (InstructionSystem)
        {
            InstructionSystem->NextInstruction();
            UE_LOG(LogTemp, Warning, TEXT("NextInstruction called from StartSpray"));
        }
    }
}

void AVRFireExtinguisher::StopSpray()
{
    bIsSpraying = false;
    SprayParticle->Deactivate();
    UE_LOG(LogTemp, Warning, TEXT("Stopped spraying!"));
}

void AVRFireExtinguisher::SprayTick(float DeltaTime)
{
    TArray<AActor*> FireActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(),
        AVRFire::StaticClass(), FireActors);

    for (AActor* Actor : FireActors)
    {
        AVRFire* Fire = Cast<AVRFire>(Actor);
        if (!Fire) continue;

        float Distance = FVector::Dist(
            GetActorLocation(), Fire->GetActorLocation());

        if (Distance <= SprayRange)
        {
            Fire->ApplyExtinguisher(DeltaTime);
            UE_LOG(LogTemp, Warning, TEXT("Extinguishing! Fire health: %f"),
                Fire->FireHealth);
        }
    }
}
