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

void Bot::OnScenarioCompleted()
{
	++scenarioIndex;
}

void Bot::OnTestCompleted()
{

}