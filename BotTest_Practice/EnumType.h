#pragma once

enum class PACKET_ID : unsigned int
{
	INVALID_PACKET = 0
	, PING
	, PONG
	, TEST_STRING_PACKET
	, ECHO_STRING_PACEKT
};