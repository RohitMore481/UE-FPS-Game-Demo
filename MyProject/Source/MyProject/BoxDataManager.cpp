#include "BoxDataManager.h"
#include "HttpModule.h"
#include "Json.h"
#include "JsonUtilities.h"

ABoxDataManager::ABoxDataManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABoxDataManager::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("BeginPlay() called on ABoxDataManager"));  // Add this line
    FetchJsonData();
}


void ABoxDataManager::FetchJsonData()
{
    FString Url = TEXT("https://raw.githubusercontent.com/CyrusCHAU/Varadise-Technical-Test/refs/heads/main/data.json");
    UE_LOG(LogTemp, Warning, TEXT("Sending HTTP request to: %s"), *Url);


    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &ABoxDataManager::OnJsonResponseReceived);
    Request->SetURL(Url);
    Request->SetVerb("GET");
    Request->ProcessRequest();
}

void ABoxDataManager::OnJsonResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Warning, TEXT("OnJsonResponseReceived called. Success: %s"), bWasSuccessful ? TEXT("true") : TEXT("false"));

    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to fetch JSON"));
        return;
    }

    FString JsonString = Response->GetContentAsString();
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        const TArray<TSharedPtr<FJsonValue>>* TypesArray;
        if (JsonObject->TryGetArrayField("types", TypesArray))
        {
            for (const auto& TypeValue : *TypesArray)
            {
                TSharedPtr<FJsonObject> TypeObj = TypeValue->AsObject();
                FBoxType TypeData;
                TypeData.Name = TypeObj->GetStringField("name");

                const TArray<TSharedPtr<FJsonValue>> ColorArray = TypeObj->GetArrayField("color");
                TypeData.Color = FLinearColor(
                    ColorArray[0]->AsNumber() / 255.0f,
                    ColorArray[1]->AsNumber() / 255.0f,
                    ColorArray[2]->AsNumber() / 255.0f
                );

                TypeData.Health = TypeObj->GetIntegerField("health");
                TypeData.Score = TypeObj->GetIntegerField("score");

                BoxTypes.Add(TypeData.Name, TypeData);
            }
        }

        const TArray<TSharedPtr<FJsonValue>>* ObjectsArray;
        if (JsonObject->TryGetArrayField("objects", ObjectsArray))
        {
            for (const auto& ObjValue : *ObjectsArray)
            {
                TSharedPtr<FJsonObject> ObjObj = ObjValue->AsObject();
                FBoxObject ObjData;
                ObjData.Type = ObjObj->GetStringField("type");

                TSharedPtr<FJsonObject> Transform = ObjObj->GetObjectField("transform");
                const TArray<TSharedPtr<FJsonValue>> Loc = Transform->GetArrayField("location");
                const TArray<TSharedPtr<FJsonValue>> Rot = Transform->GetArrayField("rotation");
                const TArray<TSharedPtr<FJsonValue>> Scale = Transform->GetArrayField("scale");

                ObjData.Location = FVector(Loc[0]->AsNumber(), Loc[1]->AsNumber(), Loc[2]->AsNumber());
                ObjData.Rotation = FRotator(Rot[1]->AsNumber(), Rot[2]->AsNumber(), Rot[0]->AsNumber());
                ObjData.Scale = FVector(Scale[0]->AsNumber(), Scale[1]->AsNumber(), Scale[2]->AsNumber());

                BoxObjects.Add(ObjData);
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Parsed %d box types and %d objects"), BoxTypes.Num(), BoxObjects.Num());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON"));
    }
}
