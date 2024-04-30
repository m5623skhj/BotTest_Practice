#include "PreCompile.h"
#include "Action.h"
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

	actionScenario[index].second->DoAction(targetBot);
}

bool BotActionManager::ReadBotTestScenario()
{
	auto jsonData = OpenTestScenarioJson();
	if (jsonData == std::nullopt)
	{
		return false;
	}

	return MakeTestScenarioObject(jsonData.value());
}

std::optional<nlohmann::json> BotActionManager::OpenTestScenarioJson()
{
	std::ifstream scenarioFile(scenarioFilePath);
	if (scenarioFile.is_open() == false)
	{
		std::cout << "Invalid scenario path : " << scenarioFilePath << std::endl;
		return std::nullopt;
	}

	nlohmann::json jsonData;
	try
	{
		scenarioFile >> jsonData;
	}
	catch (nlohmann::json::parse_error& e)
	{
		std::cout << "Parsing error : " << e.what() << std::endl;
		return std::nullopt;
	}

	if (not jsonData.is_object() || not jsonData.contains("Scenario"))
	{
		std::cout << "Invalid json format. 'scenario' array not found." << std::endl;
		return std::nullopt;
	}

	return jsonData;
}

bool BotActionManager::MakeTestScenarioObject(const nlohmann::json& testScenarioJson)
{
	for (const auto& scenarioJson : testScenarioJson["Scenario"])
	{
		if (not scenarioJson.contains("Action"))
		{
			std::cout << "Action not found in json object." << std::endl;
			continue;
		}

		std::string actionString = scenarioJson["Action"].get<std::string>();
		std::shared_ptr<IBotAction> actionObject = MakeBotActionObject(actionString);
		if (actionObject == nullptr)
		{
			std::cout << "Action string " << actionString << "is invalid" << std::endl;
			return false;
		}
		actionObject->InitAction(scenarioJson);

		actionScenario.emplace_back(actionString, actionObject);
	}

	return true;
}

std::shared_ptr<IBotAction> BotActionManager::MakeBotActionObject(std::string_view actionString)
{
	auto itor = actionFactoryMap.find(actionString);
	if (itor == actionFactoryMap.end())
	{
		return nullptr;
	}

	return itor->second();
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

void BotActionKeyword_LoopStart::InitAction(const nlohmann::json& josn)
{

}

void BotActionKeyword_LoopStart::DoAction(Bot& targetBot)
{

}

void BotActionKeyword_LoopEnd::InitAction(const nlohmann::json& josn)
{

}

void BotActionKeyword_LoopEnd::DoAction(Bot& targetBot)
{

}
#pragma endregion BotAction
