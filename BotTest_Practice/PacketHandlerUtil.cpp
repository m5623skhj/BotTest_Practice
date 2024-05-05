#include "PreCompile.h"
#include "PacketHandlerUtil.h"
#include <random>

namespace Local
{
    std::string gen_random(const int stringMaxLength)
    {
        static const char stringItems[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        std::string tempString;
        tempString.reserve(stringMaxLength);

        for (int i = 0; i < stringMaxLength; ++i)
        {
            tempString += stringItems[rand() % (sizeof(stringItems) - 1)];
        }

        return tempString;
    }
}

namespace PacketHandlerUtil
{
	std::string MakeRandomString(const int stringMaxLength)
	{
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(1, stringMaxLength);
        return Local::gen_random(distr(gen));
	}
}