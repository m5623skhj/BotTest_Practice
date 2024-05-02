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
	return BOT_POST_ACTION::DO_NEXT_IMMEDIATLY;
}

void BotActionKeyword_LoopEnd::InitAction(const nlohmann::json& json)
{
	loopCount = json["LoopCount"];
}

BOT_POST_ACTION BotActionKeyword_LoopEnd::DoAction(Bot& targetBot)
{
	if (targetBot.GetLoopCount() < loopCount)
	{
		auto nextJumpIndex = BotActionManager::GetInst().GetTargetJumpScenario(scenarioIndex);
		if (nextJumpIndex == std::nullopt)
		{
			std::cout << "LoopEnd : request next jump index " << scenarioIndex << " is invalid" << std::endl;
			return BOT_POST_ACTION::STOP;
		}

		targetBot.AccumulateLoopCount();
		targetBot.SetScenarioIndex(nextJumpIndex.value());
	}
	else
	{
		targetBot.InitializeLoopCount();
	}

	return BOT_POST_ACTION::DO_NEXT_IMMEDIATLY;
}
#pragma endregion BotAction
