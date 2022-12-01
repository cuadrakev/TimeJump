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
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void ClearRenderTexture();
    
    UPROPERTY(BlueprintReadWrite)
    FVector Offset;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ATwoWayGateBase *NewVar_0;
};
