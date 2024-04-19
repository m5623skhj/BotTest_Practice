#pragma once

class BotTest
{
private:
	BotTest() = default;
	~BotTest() = default;	
	BotTest(const BotTest& other) = delete; \
	BotTest& operator=(const BotTest& other) = delete; \

public:
	static BotTest& GetInstance()
	{
		static BotTest instance;
		return instance;
	}

public:

private:

};