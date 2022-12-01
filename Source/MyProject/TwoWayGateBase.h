#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TwoWayGateBase.generated.h"

UCLASS()
class MYPROJECT_API ATwoWayGateBase : public AActor
{
    GENERATED_BODY()
    
public:
    ATwoWayGateBase();
    
    FVector getOffset() const;
    
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void SetRenderTexture(UTexture* RenderTexture);
    
    FVector Offset;
    ATwoWayGateBase *NewVar_0;
};
