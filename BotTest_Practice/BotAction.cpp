#include "PreCompile.h"
#include "BotAction.h"
#include "PacketManager.h"

#pragma region BotAction
void BotAction_Ping::DoAction(Bot& targetBot)
{
	if (not PacketManager::GetInst().HandlePing(targetBot))
	{
		std::cout << "Ping action return falied" << std::endl;
		return;
	}
}

void BotActionKeyword_LoopStart::InitAction(const nlohmann::json& json)
{
	loopCount = json["LoopCount"];
}

void BotActionKeyword_LoopStart::DoAction(Bot& targetBot)
{

}

void BotActionKeyword_LoopEnd::InitAction(const nlohmann::json& json)
{

}

void BotActionKeyword_LoopEnd::DoAction(Bot& targetBot)
{

}
#pragma endregion BotAction
