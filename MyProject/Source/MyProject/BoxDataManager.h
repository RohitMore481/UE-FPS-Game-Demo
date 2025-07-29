#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Http.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "BoxDataManager.generated.h"

USTRUCT()
struct FBoxType
{
    GENERATED_BODY()

    FString Name;
    FLinearColor Color;
    int32 Health;
    int32 Score;
};

USTRUCT()
struct FBoxObject
{
    GENERATED_BODY()

    FString Type;
    FVector Location;
    FRotator Rotation;
    FVector Scale;
};

UCLASS()
class MYPROJECT_API ABoxDataManager : public AActor
{
    GENERATED_BODY()

public:
    ABoxDataManager();

protected:
    virtual void BeginPlay() override;

private:
    void FetchJsonData();
    void OnJsonResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    TMap<FString, FBoxType> BoxTypes;
    TArray<FBoxObject> BoxObjects;
};
