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

    // TriggerMesh 초기화
    TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
    TriggerMesh->SetupAttachment(RootComponent); // RootComponent에 붙임
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Path/To/Your/StaticMesh.StaticMesh'")); // 실제 경로로 수정
    if (MeshAsset.Succeeded())
    {
        TriggerMesh->SetStaticMesh(MeshAsset.Object);
        TriggerMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f)); // 원하는 위치로 조정
        TriggerMesh->SetWorldScale3D(FVector(1.f)); // 스케일 조정
    }
}

void ABaseTriggerBox::BeginPlay()
{
    Super::BeginPlay();
    //mesh야 제발 보여라 얍!
    if (TriggerMesh)
    {
        TriggerMesh->SetVisibility(true);
    }
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
            APlayerController* PlayerController = Cast<APlayerController>(A_WSCharacter->GetController());
            if (PlayerController)
            {
                FInputModeGameOnly InputMode;
                PlayerController->SetInputMode(InputMode);
                PlayerController->bShowMouseCursor = false;
                A_WSCharacter->EnableInput(PlayerController);
            }
        }
    }
}

void ABaseTriggerBox::ShowPortalUI(AA_WSCharacter* A_WSCharacter)
{
    if (PortalUIClass && !PortalUI) // PortalUIClass가 nullptr이 아닐 때
    {
        PortalUI = CreateWidget<UUserWidget>(GetWorld(), PortalUIClass); // UPortalUI가 아닌 UUserWidget으로 변경
        if (PortalUI)
        {
            PortalUI->AddToViewport();

            // 버튼 클릭 이벤트 바인딩
            UButton* YesButton = Cast<UButton>(PortalUI->GetWidgetFromName(TEXT("YesButton")));
            UButton* NoButton = Cast<UButton>(PortalUI->GetWidgetFromName(TEXT("NoButton")));
            if (YesButton)
            {
                YesButton->OnClicked.AddDynamic(this, &ABaseTriggerBox::OnYesClicked);
            }
            if (NoButton)
            {
                NoButton->OnClicked.AddDynamic(this, &ABaseTriggerBox::OnNoClicked);
            }

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
    // "Dungeon" 레벨로 이동
    UGameplayStatics::OpenLevel(GetWorld(), FName("Dungeon"));

    // 입력 모드 복구
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        FInputModeGameOnly InputMode;
        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = false; // 마우스 커서 숨김
    }
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
