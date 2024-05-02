#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "BotAction.h"
#include "Define.h"

using ActionFactoryFunction = std::function<std::shared_ptr<IBotAction>()>;

class BotActionManager
{
private:
	BotActionManager() = default;
	~BotActionManager() = default;
	BotActionManager(const BotActionManager& other) = delete;
	BotActionManager& operator=(const BotActionManager& other) = delete;

public:
	static BotActionManager& GetInst();

public:
	template <typename BotActionType>
	void RegisterBotAction(const std::string& key)
	{
		static_assert(std::is_base_of<IBotAction, BotActionType>::value, "RegisterBotAction must ingerit from IBotAction");
		ActionFactoryFunction func = []()
		{
			return std::make_shared<BotActionType>();
		};

		actionFactoryMap[key] = func;
	}

public:
	bool Initialize();

public:
	void DoBotAction(Bot& targetBot);

private:
	void PostProcessBotAction(Bot& targetBot, BOT_POST_ACTION actionCompletedType);
	bool ReadBotTestScenario();
	std::optional<nlohmann::json> OpenTestScenarioJson();
	bool MakeTestScenarioObject(const nlohmann::json& testScenarioJson);
	std::shared_ptr<IBotAction> MakeBotActionObject(const std::string& actionString);

private:
	std::vector<std::pair<std::string, std::shared_ptr<IBotAction>>> actionScenario;
	std::unordered_map<std::string, ActionFactoryFunction> actionFactoryMap;

	const std::string scenarioFilePath = "BotScenario.json";

public:
	std::optional<ScenarioIndex> GetTargetJumpScenario(ScenarioIndex nowIndex);

private:
	bool GenerateScenarioLoopMap();

private:
	std::unordered_map<ScenarioIndex, ScenarioIndex> scenarioJumpMap;
};