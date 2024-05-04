#include "PreCompile.h"
#include <iostream>
#include "BotTest.h"

bool Initialize(const std::wstring& optionFile)
{
	if (BotTest::GetInst().Start(optionFile) == false)
	{
		return false;
	}

	return true;
}

void InputButton()
{

}

int main()
{
	std::cout << "--------------------------------" << std::endl;
	std::cout << "Initialize bot test" << std::endl;
	std::cout << "--------------------------------" << std::endl << std::endl << std::endl;
	if (Initialize(L"") == false)
	{
		std::cout << "Initialize bot test failed" << std::endl;
		return 0;
	}

	std::cout << "--------------------------------" << std::endl;
	std::cout << "Start bot test" << std::endl;
	std::cout << "--------------------------------" << std::endl << std::endl << std::endl;

	
	std::cout << "--------------------------------" << std::endl;
	std::cout << "End bot test" << std::endl;
	std::cout << "--------------------------------" << std::endl << std::endl << std::endl;

	return 0;
}