#pragma once
#include "../Core/SDK.h"
#include "../pch.h"

#include "../Menu/Menu.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace ESP
{
    // ESP Helper Functions
    void RenderESPDistance(SDK::FVector2D screenPos, float distance, SDK::FLinearColor* staticColor, float gradientOffset = 0.0f, float yOffset = 15.0f);
    void RenderESPName(SDK::FVector2D screenPos, const std::string& name, SDK::FLinearColor* staticColor, float gradientOffset = 0.0f);
    void RenderESPBox(SDK::FVector2D screenPos, float boxSize, SDK::FLinearColor* staticColor, float gradientOffset = 0.0f);
    void RenderESP3DBox(SDK::FVector actorPos, SDK::FVector boxExtent, SDK::FRotator rotation, SDK::FLinearColor* staticColor, float gradientOffset = 0.0f);
    void RenderESPSphere(SDK::FVector actorPos, float radius, SDK::FLinearColor* staticColor, float gradientOffset = 0.0f);
    void RenderESPCircle(SDK::FVector2D screenPos, float radius, SDK::FLinearColor* staticColor, float gradientOffset = 0.0f);
    void RenderESPLine(SDK::FVector2D startPos, SDK::FVector2D endPos, SDK::FLinearColor* staticColor, float gradientOffset = 0.0f);


    // Combined ESP Rendering Functions
    void RenderPlayerESP(TFD::AM1Player* player, SDK::FVector2D screenPos);
    void RenderEnemyESP(TFD::AM1Character* enemy, SDK::FVector2D screenPos);
    void RenderLootESP(TFD::AActor* lootActor, SDK::FVector2D screenPos, const std::string& itemName, SDK::FLinearColor* itemColor);

    // Main ESP Functions
    void FriendlyESP();
    void EnemyESP();
    void LootESP();
    void CollectibleESP();
}
