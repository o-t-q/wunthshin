#include "BaseTriggerBox.h"
#include "Components/BoxComponent.h"
#include "Components/Button.h"
#include "../../Actors/AA_WSCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ABaseTriggerBox::ABaseTriggerBox()
{
    // ActiveCollisionComponent 초기화
    ActiveCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ActiveCollisionComp"));
    ActiveCollisionComponent->SetupAttachment(RootComponent);

    // 충돌 프로필 수정
    ActiveCollisionComponent->SetCollisionObjectType(ECC_Pawn);
    ActiveCollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    ActiveCollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    ActiveCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnActivate);
    ActiveCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnDeactivate);
}

void ABaseTriggerBox::BeginPlay()
{
    Super::BeginPlay();
}

void ABaseTriggerBox::OnActivate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        AA_WSCharacter* A_WSCharacter = Cast<AA_WSCharacter>(OtherActor);
        if (A_WSCharacter)
        {
            ShowPortalUI(A_WSCharacter); // PlayerCharacter를 인자로 전달
        }
    }
}

void ABaseTriggerBox::OnDeactivate(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (PortalUI)
    {
        PortalUI->RemoveFromParent();
        PortalUI = nullptr;

        if (AA_WSCharacter* A_WSCharacter = Cast<AA_WSCharacter>(OtherActor))
        {
            // GetController를 APlayerController로 캐스팅
            APlayerController* PlayerController = Cast<APlayerController>(A_WSCharacter->GetController());
            if (PlayerController)
            {
                // 마우스 커서 비활성화 및 게임 모드로 복귀
                FInputModeGameOnly InputMode;
                PlayerController->SetInputMode(InputMode);
                PlayerController->bShowMouseCursor = false;

                // 캐릭터의 입력 재활성화
                A_WSCharacter->EnableInput(PlayerController);
            }
        }
    }
}


void ABaseTriggerBox::ShowPortalUI(AA_WSCharacter* A_WSCharacter)
{
    if (PortalUIClass) // PortalUIClass가 nullptr이 아닐 때
    {
        PortalUI = CreateWidget<UUserWidget>(GetWorld(), PortalUIClass); // UPortalUI가 아닌 UUserWidget으로 변경
        if (PortalUI)
        {
            PortalUI->AddToViewport();

            // 마우스 커서 표시 및 입력 모드 설정
            APlayerController* PlayerController = Cast<APlayerController>(A_WSCharacter->GetController());
            if (PlayerController)
            {
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(PortalUI->TakeWidget());
                PlayerController->SetInputMode(InputMode);
                PlayerController->bShowMouseCursor = true;
            }

            // 캐릭터의 입력 비활성화
            A_WSCharacter->DisableInput(PlayerController);
        }
    }
}


void ABaseTriggerBox::OnYesClicked()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName("YourLevelName")); // 이동할 레벨 이름으로 변경

    // 입력 모드 복구
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        FInputModeGameOnly InputMode;
        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = false; // 마우스 커서 숨김
    }

    /*  //캐릭터 움직임 활성화
     if (AA_WSCharacter* A_WSCharacter = cast<AA_WSCharacter>(getowningplayerpawn()))
     {
         a_wscharacter->getcharactermovement()->setmovementmode(emovementmode::move_walking);
     }*/
}

void ABaseTriggerBox::OnNoClicked()
{
    if (PortalUI)
    {
        PortalUI->RemoveFromParent();
        PortalUI = nullptr;

        // 플레이어 컨트롤러 가져오기
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            // 입력 모드 설정: 게임 전용
            FInputModeGameOnly InputMode;
            PlayerController->SetInputMode(InputMode);
            PlayerController->bShowMouseCursor = false; // 마우스 커서 숨김

            // 캐릭터의 입력 활성화
            if (AA_WSCharacter* A_WSCharacter = Cast<AA_WSCharacter>(PlayerController->GetPawn()))
            {
                A_WSCharacter->EnableInput(PlayerController);
            }
        }
    }
}