#include "PreCompile.h"
#include "BotAction.h"
#include "PacketManager.h"
#include "BotActionManager.h"

#pragma region BotAction
BOT_POST_ACTION BotAction_Ping::DoAction(Bot& targetBot)
{
	if (not PacketManager::GetInst().HandlePing(targetBot))
	{
		std::cout << "Ping action return falied" << std::endl;
		return BOT_POST_ACTION::STOP;
	}

	return BOT_POST_ACTION::NORMAL;
}

void BotActionKeyword_LoopStart::InitAction(const nlohmann::json& json)
{

}

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
#pragma endregion BotAction
