#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "BaseTriggerBox.generated.h"

UCLASS()
class WUNTHSHIN_API ABaseTriggerBox : public ATriggerBox
{
    GENERATED_BODY()

public:
    ABaseTriggerBox();

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnActivate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnDeactivate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowPortalUI(AA_WSCharacter* A_WSCharacter);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void OnYesClicked();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void OnNoClicked();

    bool bIsUIVisible;

protected:
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> PortalUIClass;

    UPROPERTY()
    UUserWidget* PortalUI;

    UPROPERTY(VisibleAnywhere)
    UBoxComponent* ActiveCollisionComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FSoftObjectPath SelectedLevel;  
};
