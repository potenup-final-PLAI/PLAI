// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle/BattlePlayer/Player/BattlePlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Battle/TurnSystem/BattlePlayerController.h"
#include "Battle/TurnSystem/PhaseManager.h"
#include "Battle/Util/DebugHeader.h"
#include "Components/RectLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Enemy/BaseEnemy.h"
#include "Player/BattlePlayerAnimInstance.h"

ABattlePlayer::ABattlePlayer()
{
	// 플레이어 세팅
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetRelativeLocationAndRotation(FVector(0, 0, -100),FRotator(0, -90, 0));
	meshComp->bReceivesDecals = false;
	// Mesh Setting
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("'/Game/ParagonGreystone/Characters/Heroes/Greystone/Meshes/Greystone.Greystone'"));
	if (tempMesh.Succeeded())
	{
		meshComp->SetSkeletalMesh(tempMesh.Object);
	}

	// 라이트 세팅
	firstRectLightComp = CreateDefaultSubobject<URectLightComponent>(TEXT("firstRectLightComp"));
	firstRectLightComp->SetupAttachment(RootComponent);
	firstRectLightComp->SetRelativeLocationAndRotation(FVector(108, 51, 76), FRotator(0, -120, 0));
	firstRectLightComp->SetIntensity(500);
	firstRectLightComp->SetAttenuationRadius(200);

	secondRectLightComp = CreateDefaultSubobject<URectLightComponent>(TEXT("secondRectLightComp"));
	secondRectLightComp->SetupAttachment(RootComponent);
	secondRectLightComp->SetRelativeLocationAndRotation(FVector(92, -30, 57), FRotator(0, -230, 0));
	secondRectLightComp->SetIntensity(500);
	secondRectLightComp->SetAttenuationRadius(200);

	thirdRectLightComp = CreateDefaultSubobject<URectLightComponent>(TEXT("thirdRectLightComp"));
	thirdRectLightComp->SetupAttachment(RootComponent);
	thirdRectLightComp->SetRelativeLocationAndRotation(FVector(-83, -14, 57), FRotator(0, -360, 0));
	thirdRectLightComp->SetIntensity(500);
	thirdRectLightComp->SetAttenuationRadius(200);

	spotLightComp = CreateDefaultSubobject<USpotLightComponent>(TEXT("spotLightComp"));
	spotLightComp->SetupAttachment(RootComponent);
	spotLightComp->SetRelativeLocationAndRotation(FVector(0, 0, 323), FRotator(-90, 360, -360));
	spotLightComp->SetIntensity(1000);
	spotLightComp->SetAttenuationRadius(550);
	spotLightComp->SetOuterConeAngle(14);
	
	// Animation Instance 세팅
	ConstructorHelpers::FClassFinder<UAnimInstance> tempAnimInstance(TEXT("'/Game/JS/Blueprints/Animation/ABP_BattlePlayer.ABP_BattlePlayer_C'"));
	if (tempAnimInstance.Succeeded())
	{
		meshComp->SetAnimInstanceClass(tempAnimInstance.Class);
	}
	
	bReplicates = true;
}

void ABattlePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	playerAnim = Cast<UBattlePlayerAnimInstance>(meshComp->GetAnimInstance());
}

void ABattlePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::SpaceBar))
	{
		if (HasAuthority())
		{
			GetWorld()->ServerTravel(TEXT("/Game/Mk_Item/Mk_WorldPartition?listen"));
		}
	}
}

void ABattlePlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	UE_LOG(LogTemp, Warning,TEXT("NewController : %s"), *NewController->GetActorNameOrLabel());
	
}

void ABattlePlayer::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	
	auto* ppc = Cast<APlayerController>(Controller);

	if (ppc)
	{
		NET_PRINTLOG(TEXT("ABattlePlayer::NotifyControllerChanged"));
		//그 객체를 이용해서 EnhanceInputLocalPlayerSubSystem을 가져온다.
		UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ppc->GetLocalPlayer());
		if (subSys)
		{
			subSys->RemoveMappingContext(IMC_Player);
			subSys->AddMappingContext(IMC_Player, 0);
		}
	}
	else
	{
		NET_PRINTLOG(TEXT("PC is Nullptr"));
	}

	
}

void ABattlePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* pi = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		pi->BindAction(IA_Move, ETriggerEvent::Completed, this, &ABaseBattlePawn::OnMouseLeftClick);
	}
}

void ABattlePlayer::ClearDebugNetLog()
{
	phaseManager->ClearNetLog();
}

void ABattlePlayer::Server_OnClickedMove_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Move"));
	// 애니메이션 세팅
	currentActionMode = EActionMode::Move;

	bIsMoveMode = true;
	
	// 범위 보이게 설정
	Multicast_SeeMoveRange();
	UE_LOG(LogTemp, Warning, TEXT("move_Range %d"), moveRange);
}

void ABattlePlayer::Server_OnClickedTurnEnd_Implementation()
{
	if (phaseManager->currentPhase == EBattlePhase::BattleEnd)
	{
		UE_LOG(LogTemp, Warning, TEXT("Block!!! Battle End"));
		return;
	}
	if (phaseManager == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActionUI OnClickedTurnEnd phaseManager nullPtr"));
		return;
	}
	// 버튼 상태 업데이트
	this->currentActionMode = EActionMode::TurnEnd;
	
	// 플레이어 턴 종료
	NET_PRINTLOG(TEXT("Click Turn End Player"));
	turnManager->bTurnEnded = false;
	turnManager->OnTurnEnd();
}

void ABattlePlayer::Server_OnClickedBaseAttack_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("FirstSkill"));
	currentActionMode = EActionMode::BaseAttack;
}

void ABattlePlayer::Server_OnClickedPoison_Implementation()
{

	turnManager->curUnit->currentActionMode = EActionMode::Poison;
}

void ABattlePlayer::Server_OnClickedFatal_Implementation()
{
	turnManager->curUnit->currentActionMode = EActionMode::Fatal;
}

void ABattlePlayer::Server_OnClickedRupture_Implementation()
{
	turnManager->curUnit->currentActionMode = EActionMode::Rupture;
}
void ABattlePlayer::Server_UpdatePlayerAnim_Implementation(EActionMode mode)
{
	MultiCastRPC_UpdatePlayerAnim(mode);
}
void ABattlePlayer::MultiCastRPC_UpdatePlayerAnim_Implementation(EActionMode mode)
{
	if (playerAnim)
	{
		playerAnim->actionMode = mode;
		NET_PRINTLOG(TEXT("호출한 객체 : %s, currentActionMode : %s, playerAnim->actionMode : %s"), *GetActorNameOrLabel(), *UEnum::GetValueAsString(currentActionMode), *UEnum::GetValueAsString(playerAnim->actionMode));
		switch (mode)
		{
			case EActionMode::Move:
				break;
			case EActionMode::BaseAttack:
				bStartMontage = true;
				break;
			case EActionMode::Poison:
				playerAnim->PlayerAttackAnimation(TEXT("StartPoison"));
				break;
			case EActionMode::Fatal:
				playerAnim->PlayerAttackAnimation(TEXT("StartFatal"));
				break;
			case EActionMode::Rupture:
				playerAnim->PlayerAttackAnimation(TEXT("StartRupture"));
				break;
			default:
				break;
		}
	}
}

void ABattlePlayer::Server_PlayerTryConsumeAP_Implementation(int32 amount)
{
	// if (CanConsumeAP(amount))
	// {
	// 	ApplyConsumeAP(amount);
	// }
	
}

void ABattlePlayer::Multicast_PlayerTryConsumeAP_Implementation(int32 amount)
{
	
}
