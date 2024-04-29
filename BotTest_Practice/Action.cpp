#include "PreCompile.h"
#include "Action.h"
#include "nlohmann/json.hpp"
#include "Bot.h"
#include "PacketManager.h"
#include <fstream>

BotActionManager& BotActionManager::GetInst()
{
	static BotActionManager instance;
	return instance;
}

void BotActionManager::Initialize()
{
	if (ReadBotTestScenario() == false)
	{
		throw;
	}
}

void BotActionManager::DoBotAction(Bot& targetBot)
{
	auto index = targetBot.GetScenarioIndex();
	if (actionScenario.size() <= index)
	{
		targetBot.OnTestCompleted();
		return;
	}

	auto itor = actionFactoryMap[actionScenario[index]];
	if (itor == nullptr)
	{
		std::cout << "DoBotAction() : Scenario " << index << " factory return nullptr" << std::endl;
		throw;
	}

	itor()->DoAction(targetBot);
}

bool BotActionManager::ReadBotTestScenario()
{
	std::ifstream scenarioFile(scenarioFilePath);
	if (scenarioFile.is_open() == false)
	{
		std::cout << "Invalid scenario path : " << scenarioFilePath << std::endl;
		return false;
	}

	nlohmann::json jsonData;
	try
	{
		scenarioFile >> jsonData;
	}
	catch (nlohmann::json::parse_error& e)
	{
		std::cout << "Parsing error : " << e.what() << std::endl;
		return false;
	}

	if (not jsonData.is_object() || not jsonData.contains("Scenario"))
	{
		std::cout << "Invalid json format. 'scenario' array not found." << std::endl;
		return false;
	}

	for (const auto& action : jsonData["Scenario"])
	{
		if (not action.contains("Action"))
		{
			std::cout << "Action not found in json object." << std::endl;
			continue;
		}

		actionScenario.emplace_back(action["Action"].get<std::string>());
	}

	return true;
}

#pragma region BotAction
void BotAction_Ping::DoAction(Bot& targetBot)
{
	if (not PacketManager::GetInst().HandlePing(targetBot))
	{
		std::cout << "Ping action return falied" << std::endl;
		return;
	}
}
#pragma endregion BotAction
