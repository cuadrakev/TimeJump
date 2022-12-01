#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "PortalViewRenderer.generated.h"

class ATwoWayGateBase;

UCLASS()
class MYPROJECT_API APortalViewRenderer : public AActor
{
    GENERATED_BODY()
    
public:
    APortalViewRenderer();
    
    UFUNCTION(BlueprintCallable)
    void init();
    UFUNCTION(BlueprintCallable)
    void setOwnerController(APlayerController *ownerController);
    
    // called by character class during its tick
    UFUNCTION(BlueprintCallable)
    void update(float deltaTime);
    

private:
    void generateRenderTexture();
    ATwoWayGateBase *selectPortal();
    void sceneCapture(ATwoWayGateBase *portal);

    UPROPERTY()
    APlayerController *m_ownerControllerp;
    ACharacter *character;
    USceneCaptureComponent2D *sceneCaptureComponent;
    UTextureRenderTarget2D *portalTexture;
    FIntVector lastScreenSize;

};
