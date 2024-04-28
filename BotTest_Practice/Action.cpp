#include "PreCompile.h"
#include "Action.h"
#include "nlohmann/json.hpp"
#include "Bot.h"
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

	itor()->DoAction();
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

	for (const auto& json : jsonData)
	{
		actionScenario.emplace_back(json["item"]);
	}

	return true;
}

#pragma region BotAction
void BotAction_Ping::DoAction()
{

}
#pragma endregion BotAction
