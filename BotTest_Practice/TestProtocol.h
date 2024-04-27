#pragma once
#include "EnumType.h"
#include <string>

using PacketId = unsigned int;

#define GET_PACKET_ID(packetId) virtual PacketId GetPacketId() const override { return static_cast<PacketId>(packetId); }

struct IPacket
{
public:
	IPacket() = default;
	virtual ~IPacket() = default;

	virtual PacketId GetPacketId() const = 0;
};

class Ping : public IPacket
{
public:
	GET_PACKET_ID(PACKET_ID::PING);
};

class Pong : public IPacket
{
public:
	GET_PACKET_ID(PACKET_ID::PONG);
};

class TestStringPacket : public IPacket
{
public:
	GET_PACKET_ID(PACKET_ID::TEST_STRING_PACKET);

public:
	std::string testString;
};

class EchoStringPacket : public IPacket
{
public:
	GET_PACKET_ID(PACKET_ID::ECHO_STRING_PACEKT);

public:
	char echoString[30];
};

#define DECLARE_HANDLE_PACKET(PacketType)\
	static bool Handle##PacketType(Bot& bot);\

#define DECLARE_RECV_HANDLE_PACKET(PacketType)\
	static bool Handle##PacketType(Bot& bot, NetBuffer& packet);\

#define DECLARE_ALL_HANDLER()\
	DECLARE_HANDLE_PACKET(TestStringPacket)\
	DECLARE_HANDLE_PACKET(Ping)

#define DECLARE_ALL_RECV_HANDLE_PACKET()\
	DECLARE_RECV_HANDLE_PACKET(EchoStringPacket)\
	DECLARE_RECV_HANDLE_PACKET(Pong)
