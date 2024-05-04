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

#define DECLARE_PACKET_HANDLER(PacketType)\
	static bool Handle##PacketType(Bot& bot);\

#define DECLARE_RECV_PACKET_HANDLER(PacketType)\
	static bool Handle##PacketType(Bot& bot, NetBuffer& packet);\

#define REGISTER_PACKET_HANDLER(TargetPacketId, PacketType)\
	packetHandlerMap.insert({ static_cast<PacketId>(TargetPacketId), Handle##PacketType });\

#define REGISTER_RECV_PACKET_HANDLER(TargetPacketId, PacketType)\
	recvPacketHandlerMap.insert({ static_cast<PacketId>(TargetPacketId), Handle##PacketType });\

#define REGISTER_PACKET_NAME(TargetPacketId, PacketName)\
	packetNameMap.insert({ static_cast<PacketId>(TargetPacketId), #PacketName });

#pragma region Declare
#define DECLARE_ALL_PACKET_HANDLER()\
	DECLARE_PACKET_HANDLER(TestStringPacket)\
	DECLARE_PACKET_HANDLER(Ping)\

#define DECLARE_ALL_RECV_PACKET_HANDLER()\
	DECLARE_RECV_PACKET_HANDLER(EchoStringPacket)\
	DECLARE_RECV_PACKET_HANDLER(Pong)\

#pragma endregion Declare

#pragma region Register
#define REGISTER_ALL_PACKET_HANDLER()\
	REGISTER_PACKET_HANDLER(PACKET_ID::TEST_STRING_PACKET, TestStringPacket)\
	REGISTER_PACKET_HANDLER(PACKET_ID::PING, Ping)\

#define REGISTER_ALL_RECV_PACKET_HANDLER()\
	REGISTER_RECV_PACKET_HANDLER(PACKET_ID::ECHO_STRING_PACEKT, EchoStringPacket)\
	REGISTER_RECV_PACKET_HANDLER(PACKET_ID::PONG, Pong)\

#define REGISTER_ALL_PACKET_NAME()\
	REGISTER_PACKET_NAME(PACKET_ID::TEST_STRING_PACKET, TestStringPacket)\
	REGISTER_PACKET_NAME(PACKET_ID::PING, Ping)\
	REGISTER_PACKET_NAME(PACKET_ID::ECHO_STRING_PACEKT, EchoStringPacket)\
	REGISTER_PACKET_NAME(PACKET_ID::PONG, Pong)\

#pragma endregion Register
