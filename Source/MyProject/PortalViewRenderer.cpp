#include "PortalViewRenderer.h"

#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "GameFramework/Character.h"

#include "TwoWayGateBase.h"

APortalViewRenderer::APortalViewRenderer()
{
    PrimaryActorTick.bCanEverTick = false;
    m_ownerControllerp = nullptr;
    character = nullptr;
    sceneCaptureComponent = nullptr;
    portalTexture = nullptr;
    lastScreenSize = FIntVector(0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////

void APortalViewRenderer::init()
{
    sceneCaptureComponent = NewObject<USceneCaptureComponent2D>(this,
                                USceneCaptureComponent2D::StaticClass(),
                                *FString("PortalSceneCapture"));
    
    sceneCaptureComponent->AttachToComponent(GetRootComponent(),
                            FAttachmentTransformRules::SnapToTargetIncludingScale);
    sceneCaptureComponent->RegisterComponent();
    
    sceneCaptureComponent->bCaptureEveryFrame         = false;
    sceneCaptureComponent->bCaptureOnMovement         = false;
    sceneCaptureComponent->LODDistanceFactor          = 1;
    sceneCaptureComponent->TextureTarget              = nullptr;
    sceneCaptureComponent->bEnableClipPlane           = true;
    sceneCaptureComponent->bUseCustomProjectionMatrix = true;
    sceneCaptureComponent->CaptureSource              = ESceneCaptureSource::SCS_FinalColorHDR;
    
    FPostProcessSettings postProcessSettings;
    postProcessSettings.bOverride_AmbientOcclusionQuality      = true;
    postProcessSettings.bOverride_MotionBlurAmount             = true;
    postProcessSettings.bOverride_SceneFringeIntensity         = true;
    postProcessSettings.bOverride_FilmGrainIntensity           = true;
    postProcessSettings.bOverride_ScreenSpaceReflectionQuality = true;
    postProcessSettings.AmbientOcclusionQuality      = 0.f;
    postProcessSettings.MotionBlurAmount             = 0.f;
    postProcessSettings.SceneFringeIntensity         = 0.f;
    postProcessSettings.FilmGrainIntensity           = 0.f;
    postProcessSettings.ScreenSpaceReflectionQuality = 0.f;
    
    sceneCaptureComponent->PostProcessSettings = postProcessSettings;
    
    generateRenderTexture();
}
////////////////////////////////////////////////////////////////////////////////////////////////////

void APortalViewRenderer::setOwnerController(APlayerController *ownerController)
{
    m_ownerControllerp = ownerController;
    character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////

void APortalViewRenderer::update(float deltaTime)
{
    generateRenderTexture();// if resulution hasn't changed it will simply return
    ATwoWayGateBase *portalGate = selectPortal();
    if(portalGate)
    {
        sceneCapture(portalGate);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////

void APortalViewRenderer::generateRenderTexture()
{
    FIntVector currentScreenSize(0);
    if(m_ownerControllerp != nullptr)
    {
        m_ownerControllerp->GetViewportSize(currentScreenSize.X, currentScreenSize.Y);
    }
    
    if(currentScreenSize.X == 0 || currentScreenSize == lastScreenSize)
    {
        return;
    }
    
    currentScreenSize.X = FMath::Clamp(static_cast<int32>(currentScreenSize.X * 0.8), 128, 10920);
    currentScreenSize.Y = FMath::Clamp(static_cast<int32>(currentScreenSize.Y * 0.8), 128, 10920);
    lastScreenSize = currentScreenSize;
    
    if(portalTexture == nullptr)
    {
        portalTexture = NewObject<UTextureRenderTarget2D>(this,
                            UTextureRenderTarget2D::StaticClass(),
                            *FString("PortalRenderTarget"));
        check(portalTexture);
        
        portalTexture->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
        portalTexture->Filter             = TextureFilter::TF_Bilinear;
        portalTexture->SizeX              = currentScreenSize.X;
        portalTexture->SizeY              = currentScreenSize.Y;
        portalTexture->ClearColor         = FLinearColor::Black;
        portalTexture->TargetGamma        = 2.2f;
        portalTexture->bNeedsTwoCopies    = false;
        portalTexture->AddressX           = TextureAddress::TA_Clamp;
        portalTexture->AddressY           = TextureAddress::TA_Clamp;
        portalTexture->bAutoGenerateMips  = false;
        
        portalTexture->UpdateResource();
    }
    else
    {
        portalTexture->ResizeTarget(currentScreenSize.X, currentScreenSize.Y);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////

ATwoWayGateBase *APortalViewRenderer::selectPortal()
{
    TActorIterator<ATwoWayGateBase> actorIterator(GetWorld(), ATwoWayGateBase::StaticClass(), EActorIteratorFlags::AllActors);
    ATwoWayGateBase *activePortal {nullptr};
    float minDistance = 99999.f;
    for( ; actorIterator; ++actorIterator)
    {
        ATwoWayGateBase *portal = *actorIterator;
        FVector portalLocation = portal->GetActorLocation();
        FVector playerLocation = character->GetActorLocation();
        portal->ClearRenderTexture();
        
        float distance = FMath::Abs(FVector::Dist(portalLocation, playerLocation));
        if(minDistance > distance)
        {
            minDistance = distance;
            activePortal = portal;
        }
    }
    
    return activePortal;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

void APortalViewRenderer::sceneCapture(ATwoWayGateBase *portal)
{
    if(!sceneCaptureComponent || !portalTexture || !character)
    {
        return;
    }
    
    UCameraComponent *camera = dynamic_cast<UCameraComponent*>(character->FindComponentByClass(UCameraComponent::StaticClass()));
    FVector cameraLocation = camera->GetComponentLocation();
    cameraLocation += portal->Offset;
    sceneCaptureComponent->SetWorldLocation(cameraLocation);
    sceneCaptureComponent->SetWorldRotation(camera->GetComponentRotation());
    
    FVector portalForward = portal->NewVar_0->GetActorForwardVector();
    FVector cameraForward = camera->GetForwardVector();
    double angleDistance = portalForward.CosineAngle2D(cameraForward);
    if(angleDistance >= 0)
    {
        sceneCaptureComponent->ClipPlaneNormal = portalForward;
    }
    else
    {
        sceneCaptureComponent->ClipPlaneNormal = portalForward.RotateAngleAxis(180, FVector(0.f, 0.f, 1.f));
    }
    
    sceneCaptureComponent->ClipPlaneBase = portal->NewVar_0->GetActorLocation() + (
                                           sceneCaptureComponent->ClipPlaneNormal * +1.5f);
    
    sceneCaptureComponent->TextureTarget = portalTexture;
    
    FSceneViewProjectionData projectionData;
    m_ownerControllerp->GetLocalPlayer()->GetProjectionData(m_ownerControllerp->GetLocalPlayer()->ViewportClient->Viewport,
                                        projectionData,
                                        0);
                                        
    sceneCaptureComponent->CustomProjectionMatrix = projectionData.ProjectionMatrix;
    portal->SetRenderTexture(portalTexture);
    sceneCaptureComponent->CaptureScene();
}
