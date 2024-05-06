#include "PreCompile.h"
#include "BotActionManager.h"
#include "Bot.h"
#include <fstream>
#include <stack>

BotActionManager& BotActionManager::GetInst()
{
	static BotActionManager instance;
	return instance;
}

bool BotActionManager::Initialize()
{
	REGISTER_ALL_BOT_ACTION();

	if (ReadBotTestScenario() == false)
	{
		return false;
	}

	if (GenerateScenarioLoopMap() == false)
	{
		return false;
	}

	return true;
}

void BotActionManager::DoBotAction(Bot& targetBot)
{
	auto index = targetBot.GetScenarioIndex();
	if (actionScenario.size() <= index)
	{
		targetBot.OnTestCompleted();
		return;
	}

	auto postAction = actionScenario[index].second->DoAction(targetBot);
	targetBot.AddScenarioIndex(1);
	PostProcessBotAction(targetBot, postAction);
}

void BotActionManager::PostProcessBotAction(Bot& targetBot, BOT_POST_ACTION actionCompletedType)
{
	switch (actionCompletedType)
	{
	case BOT_POST_ACTION::NORMAL:
		break;
	case BOT_POST_ACTION::DO_NEXT_IMMEDIATLY:
		DoBotAction(targetBot);
		break;
	case BOT_POST_ACTION::STOP:
		targetBot.StopBot();
		break;
	default:
		std::cout << "Invalid bot action completed type " << actionCompletedType 
			<< "in " << targetBot.GetScenarioIndex() << std::endl;
		break;
	}
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
	ScenarioIndex scenarioIndex{};
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
		if (not actionObject->InitAction(scenarioJson))
		{
			std::cout << "Scenario action index " << scenarioIndex << " was InitAction() failed" << std::endl;
			return false;
		}
		actionObject->SetScenarioIndex(scenarioIndex);

		actionScenario.emplace_back(actionString, actionObject);
		++scenarioIndex;
	}

	return true;
}

std::shared_ptr<IBotAction> BotActionManager::MakeBotActionObject(const std::string& actionString)
{
	auto itor = actionFactoryMap.find(actionString);
	if (itor == actionFactoryMap.end())
	{
		return nullptr;
	}

	return itor->second();
}

std::optional<ScenarioIndex> BotActionManager::GetTargetJumpScenario(ScenarioIndex nowIndex)
{
	auto itor = scenarioJumpMap.find(nowIndex);
	if (itor == scenarioJumpMap.end())
	{
		std::cout << "Target jump scenario is invalid " << nowIndex << std::endl;
		return std::nullopt;
	}

	return itor->second;
}

bool BotActionManager::GenerateScenarioLoopMap()
{
	std::stack<ScenarioIndex> loopStartStack;
	for (size_t scenarioIndex = 0; scenarioIndex < actionScenario.size(); ++scenarioIndex)
	{
		auto const& actionString = actionScenario[scenarioIndex].first;
		if (actionString == "LoopStart")
		{
			loopStartStack.push(scenarioIndex);
		}
		else if (actionString == "LoopEnd")
		{
			if (loopStartStack.empty())
			{
				std::cout << "Do not matched LoopStart and LoopEnd" << std::endl;
				return false;
			}

			auto startIndex = loopStartStack.top();
			scenarioJumpMap.insert({ static_cast<ScenarioIndex>(scenarioIndex), startIndex });
			loopStartStack.pop();
		}
	}

	if (not loopStartStack.empty())
	{
		std::cout << "Do not matched LoopStart and LoopEnd" << std::endl;
		return false;
	}

	return true;
}