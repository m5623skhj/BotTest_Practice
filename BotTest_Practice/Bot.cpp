#include "PreCompile.h"
#include "Bot.h"

Bot::Bot(MultiNetSessionId inSessionId)
	: sessionId(inSessionId)
{
}

void Bot::OnConnected()
{
	isConnected = true;
}

void Bot::OnDisconnected()
{
	isConnected = false;
}

ScenarioIndex Bot::GetScenarioIndex()
{
	return scenarioIndex;
}

void Bot::AddScenarioIndex(ScenarioIndex addIndex)
{
	scenarioIndex += addIndex;
}

void Bot::SetScenarioIndex(ScenarioIndex setIndex)
{
	scenarioIndex = setIndex;
}

void Bot::OnScenarioCompleted()
{
	++scenarioIndex;
}

void Bot::OnTestCompleted()
{

}

void Bot::StopBot()
{
	stopCommanded.store(true);
}

bool Bot::IsStopCommandedBot()
{
	return stopCommanded.load();
}

#pragma region BotActionItems
void Bot::PushLoopCount()
{
	loopCountStack.push(0);
}

void Bot::PopLoopCount()
{
	loopCountStack.pop();
}

void Bot::AccumulateLoopCount()
{
	++loopCountStack.top();
}

int Bot::GetLoopCount()
{
	return loopCountStack.top();
}

#pragma endregion BotActionItems