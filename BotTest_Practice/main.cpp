#include "PreCompile.h"
#include <iostream>
#include "BotActionManager.h"

bool Initialize()
{
	if (BotActionManager::GetInst().Initialize() == false)
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
	if (Initialize() == false)
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