#pragma once
#include "EnumType.h"
#include <string>

using PacketId = unsigned int;

#define GET_PACKET_ID(InPacketId) PacketId GetPacketId() const { return static_cast<PacketId>(InPacketId); }

template<class ClassType, PacketId(ClassType::*)() const>
struct SignatureFunction_Type { using type = ClassType; };

template<class ClassType, class=ClassType>
struct SignatureFunction_Predicate : std::false_type {};
template<class ClassType>
struct SignatureFunction_Predicate<ClassType, typename SignatureFunction_Type<ClassType, &ClassType::GetPacketId>::type>
	: std::true_type {};

template<class ClassType>
constexpr bool SignatureFunction_Checker = SignatureFunction_Predicate<ClassType>::value;

template<class Derived>
struct IPacket
{
	IPacket() { static_assert(SignatureFunction_Checker<Derived>, "Classes inheriting from IPacket must define GetPacketId()"); }
	virtual ~IPacket() = default;
	PacketId GetPacketId() const { return static_cast<Derived>(this)->GetPacketId(); }
};

struct Ping : private IPacket<Ping>
{
	~Ping() override = default;
	GET_PACKET_ID(PACKET_ID::PING);
};

struct Pong : public IPacket<Pong>
{
	~Pong() override = default;
	GET_PACKET_ID(PACKET_ID::PONG);
};

struct TestStringPacket : public IPacket<TestStringPacket>
{
	~TestStringPacket() override = default;
	GET_PACKET_ID(PACKET_ID::TEST_STRING_PACKET);

public:
	std::string testString;
};

struct EchoStringPacket : public IPacket<EchoStringPacket>
{
	~EchoStringPacket() override = default;
	GET_PACKET_ID(PACKET_ID::ECHO_STRING_PACEKT);

public:
	std::string echoString;
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
