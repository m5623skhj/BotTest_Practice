#include "PreCompile.h"
#include "BotAction.h"
#include "PacketManager.h"
#include "BotActionManager.h"

#define HANDLE(HandlerTargetName, TargetBot){\
	if (not PacketManager::GetInst().Handle##HandlerTargetName(TargetBot))\
	{\
		std::cout << #HandlerTargetName << " action return false" << std::endl;\
		return BOT_POST_ACTION::STOP;\
	}\
}

#pragma region BotAction
BOT_POST_ACTION BotAction_Ping::DoAction(Bot& targetBot)
{
	HANDLE(Ping, targetBot);

	return BOT_POST_ACTION::NORMAL;
}

BOT_POST_ACTION BotAction_TestStringPacket::DoAction(Bot& targetBot)
{
	HANDLE(TestStringPacket, targetBot);

	return BOT_POST_ACTION::NORMAL;
}
#pragma endregion BotAction

#pragma region BotActionKeyword
BOT_POST_ACTION BotActionKeyword_LoopStart::DoAction(Bot& targetBot)
{
	targetBot.PushLoopCount();
	return BOT_POST_ACTION::DO_NEXT_IMMEDIATLY;
}

void BotActionKeyword_LoopEnd::InitAction(const nlohmann::json& json)
{
	loopCount = json["LoopCount"];

	auto nextJumpIndex = BotActionManager::GetInst().GetTargetJumpScenario(scenarioIndex);
	if (nextJumpIndex == std::nullopt)
	{
		std::cout << "LoopEnd : request next jump index " << scenarioIndex << " is invalid" << std::endl;
		throw;
	}

	jumpScenarioIndex = nextJumpIndex.value();
}

BOT_POST_ACTION BotActionKeyword_LoopEnd::DoAction(Bot& targetBot)
{
	if (targetBot.GetLoopCount() < loopCount)
	{
		targetBot.AccumulateLoopCount();
		targetBot.SetScenarioIndex(jumpScenarioIndex);
	}
	else
	{
		targetBot.PopLoopCount();
	}

	return BOT_POST_ACTION::DO_NEXT_IMMEDIATLY;
}
#pragma endregion BotActionKeyword
