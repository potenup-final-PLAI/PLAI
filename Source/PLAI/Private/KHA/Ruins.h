#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ruins.generated.h"

class UBoxComponent;
class UWidgetComponent;
class UUserWidget;

UCLASS()

class ARuins : public AActor
{
	GENERATED_BODY()
	
public:	
	ARuins();

protected:
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaSeconds) override;
	
	//범위 인식
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* DetectionSphere;

	UFUNCTION()
	void OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
					   const FHitResult& SweepResult);
	UFUNCTION()
	void OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//위젯 띄우기
	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* RuinsNameWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> RuinsWidgetClass;
	

};
