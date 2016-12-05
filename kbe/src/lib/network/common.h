/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2016 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KBE_NETWORK_COMMON_H
#define KBE_NETWORK_COMMON_H

// common include
#include "common/common.h"
#include "helper/debug_option.h"

namespace KBEngine { 
namespace Network
{
const uint32 BROADCAST = 0xFFFFFFFF;
const uint32 LOCALHOST = 0x0100007F;

// Message ID
typedef uint16	MessageID;

// The message length, length currently has 2, the default maximum Message length the message length
// When the required length extension when this number is exceeded, the underlying Message length1
typedef uint16	MessageLength;		// Max 65535
typedef uint32	MessageLength1;		// Maximum of 4294967295

typedef int32	ChannelID;
const ChannelID CHANNEL_ID_NULL = 0;

// Channel timeout
extern float g_channelInternalTimeout;
extern float g_channelExternalTimeout;

// Send Timeout Retry
extern uint32 g_intReSendInterval;
extern uint32 g_intReSendRetries;
extern uint32 g_extReSendInterval;
extern uint32 g_extReSendRetries;

// External channel encryption categories
extern int8 g_channelExternalEncryptType;

// Listen listen queue maximum
extern uint32 g_SOMAXCONN;

// Do not channel timeout check
#define CLOSE_CHANNEL_INACTIVITIY_DETECTION()										\
{																					\
	Network::g_channelExternalTimeout = Network::g_channelInternalTimeout = -1.0f;	\
}																					\

	
namespace udp{
}

namespace tcp{
}

// Encrypt bytes of information takes up extra storage (length + fill)
#define ENCRYPTTION_WASTAGE_SIZE			(1 + 7)

#define PACKET_MAX_SIZE						1500
#ifndef PACKET_MAX_SIZE_TCP
#define PACKET_MAX_SIZE_TCP					1460
#endif
#define PACKET_MAX_SIZE_UDP					1472

typedef uint16								PacketLength;				// Max 65535
#define PACKET_LENGTH_SIZE					sizeof(PacketLength)

#define NETWORK_MESSAGE_ID_SIZE				sizeof(Network::MessageID)
#define NETWORK_MESSAGE_LENGTH_SIZE			sizeof(Network::MessageLength)
#define NETWORK_MESSAGE_LENGTH1_SIZE		sizeof(Network::MessageLength1)
#define NETWORK_MESSAGE_MAX_SIZE			65535
#define NETWORK_MESSAGE_MAX_SIZE1			4294967295

// Game contents available package sizes
#define GAME_PACKET_MAX_SIZE_TCP			PACKET_MAX_SIZE_TCP - NETWORK_MESSAGE_ID_SIZE - \
											NETWORK_MESSAGE_LENGTH_SIZE - ENCRYPTTION_WASTAGE_SIZE

/** KBE machine port */
#define KBE_PORT_START						20000
#define KBE_MACHINE_BROADCAST_SEND_PORT		KBE_PORT_START + 86			// Machine receiving broadcast port
#define KBE_PORT_BROADCAST_DISCOVERY		KBE_PORT_START + 87
#define KBE_MACHINE_TCP_PORT				KBE_PORT_START + 88

#define KBE_INTERFACES_TCP_PORT				30099

/*
	Web message types, fixed-length or variable-length.
	If you need a custom length in length filled NETWORK INTERFACE declared in the DECLARE BEGIN.
*/
#ifndef NETWORK_FIXED_MESSAGE
#define NETWORK_FIXED_MESSAGE 0
#endif

#ifndef NETWORK_VARIABLE_MESSAGE
#define NETWORK_VARIABLE_MESSAGE -1
#endif

// Network news categories
enum NETWORK_MESSAGE_TYPE
{
	NETWORK_MESSAGE_TYPE_COMPONENT = 0,	// Component messages
	NETWORK_MESSAGE_TYPE_ENTITY = 1,	// Entity news
};

enum ProtocolType
{
	PROTOCOL_TCP = 0,
	PROTOCOL_UDP = 1,
};

enum Reason
{
	REASON_SUCCESS = 0,				 // No reason.
	REASON_TIMER_EXPIRED = -1,		 // Timer expired.
	REASON_NO_SUCH_PORT = -2,		 // Destination port is not open.
	REASON_GENERAL_NETWORK = -3,	 // The network is stuffed.
	REASON_CORRUPTED_PACKET = -4,	 // Got a bad packet.
	REASON_NONEXISTENT_ENTRY = -5,	 // Wanted to call a null function.
	REASON_WINDOW_OVERFLOW = -6,	 // Channel send window overflowed.
	REASON_INACTIVITY = -7,			 // Channel inactivity timeout.
	REASON_RESOURCE_UNAVAILABLE = -8,// Corresponds to EAGAIN
	REASON_CLIENT_DISCONNECTED = -9, // Client disconnected voluntarily.
	REASON_TRANSMIT_QUEUE_FULL = -10,// Corresponds to ENOBUFS
	REASON_CHANNEL_LOST = -11,		 // Corresponds to channel lost
	REASON_SHUTTING_DOWN = -12,		 // Corresponds to shutting down app.
	REASON_WEBSOCKET_ERROR = -13,    // html5 error.
	REASON_CHANNEL_CONDEMN = -14,	 // condemn error.
};

inline
const char * reasonToString(Reason reason)
{
	const char * reasons[] =
	{
		"REASON_SUCCESS",
		"REASON_TIMER_EXPIRED",
		"REASON_NO_SUCH_PORT",
		"REASON_GENERAL_NETWORK",
		"REASON_CORRUPTED_PACKET",
		"REASON_NONEXISTENT_ENTRY",
		"REASON_WINDOW_OVERFLOW",
		"REASON_INACTIVITY",
		"REASON_RESOURCE_UNAVAILABLE",
		"REASON_CLIENT_DISCONNECTED",
		"REASON_TRANSMIT_QUEUE_FULL",
		"REASON_CHANNEL_LOST",
		"REASON_SHUTTING_DOWN",
		"REASON_WEBSOCKET_ERROR",
		"REASON_CHANNEL_CONDEMN"
	};

	unsigned int index = -reason;

	if (index < sizeof(reasons)/sizeof(reasons[0]))
	{
		return reasons[index];
	}

	return "REASON_UNKNOWN";
}


#define NETWORK_SEND_TO_ENDPOINT(ep, op, pPacket)															\
{																											\
	int retries = 0;																						\
	Network::Reason reason;																					\
																											\
	while(true)																								\
	{																										\
		retries++;																							\
		int slen = ep->op(pPacket->data(), pPacket->totalSize());											\
																											\
		if(slen != (int)pPacket->totalSize())																\
		{																									\
			reason = Network::PacketSender::checkSocketErrors(ep, slen, pPacket->totalSize());				\
			/* If you send the error so we can continue to try more than 3 times to quit	*/										\
			if (reason == Network::REASON_NO_SUCH_PORT && retries <= 3)										\
			{																								\
				continue;																					\
			}																								\
																											\
			/* If the send buffer is full, then we wait for 10ms	*/													\
			if ((reason == REASON_RESOURCE_UNAVAILABLE || reason == REASON_GENERAL_NETWORK)					\
															&& retries <= 3)								\
			{																								\
				WARNING_MSG(fmt::format("{}: "																\
					"Transmit queue full, waiting for space... ({})\n",										\
					__FUNCTION__, retries));																\
																											\
				KBEngine::sleep(10);																		\
				continue;																					\
			}																								\
																											\
			if(retries > 3 && reason != Network::REASON_SUCCESS)											\
			{																								\
				ERROR_MSG(fmt::format("NETWORK_SEND::send: packet discarded(reason={}).\n",					\
															(reasonToString(reason))));						\
				break;																						\
			}																								\
		}																									\
		else																								\
		{																									\
			break;																							\
		}																									\
	}																										\
																											\
}																											\


#define SEND_BUNDLE_COMMON(SND_FUNC, BUNDLE)																\
	BUNDLE.finiMessage();																					\
																											\
	Network::Bundle::Packets::iterator iter = BUNDLE.packets().begin();										\
	for (; iter != BUNDLE.packets().end(); ++iter)															\
	{																										\
		Packet* pPacket = (*iter);																			\
		int retries = 0;																					\
		Reason reason;																						\
		pPacket->sentSize = 0;																				\
																											\
		while(true)																							\
		{																									\
			++retries;																						\
			int slen = SND_FUNC;																			\
																											\
			if(slen > 0)																					\
				pPacket->sentSize += slen;																	\
																											\
			if(pPacket->sentSize != pPacket->length())														\
			{																								\
				reason = PacketSender::checkSocketErrors(&ep);												\
				/* If you send the error so we can continue to try again, more than 60 times to quit	*/									\
				if (reason == REASON_NO_SUCH_PORT && retries <= 3)											\
				{																							\
					continue;																				\
				}																							\
																											\
				/* If the send buffer is full, then we wait for 10ms	*/												\
				if ((reason == REASON_RESOURCE_UNAVAILABLE || reason == REASON_GENERAL_NETWORK)				\
																					&& retries <= 60)		\
				{																							\
					WARNING_MSG(fmt::format("{}: "															\
						"Transmit queue full, waiting for space... ({})\n",									\
						__FUNCTION__, retries));															\
																											\
					ep.waitSend();																			\
					continue;																				\
				}																							\
																											\
				if(retries > 60 && reason != REASON_SUCCESS)												\
				{																							\
					ERROR_MSG(fmt::format("Bundle::basicSendWithRetries: packet discarded(reason={}).\n",	\
															(reasonToString(reason))));						\
					break;																					\
				}																							\
			}																								\
			else																							\
			{																								\
				break;																						\
			}																								\
		}																									\
																											\
	}																										\
																											\
	BUNDLE.clearPackets();																					\
																											\


#define SEND_BUNDLE(ENDPOINT, BUNDLE)																		\
{																											\
	EndPoint& ep = ENDPOINT;																				\
	SEND_BUNDLE_COMMON(ENDPOINT.send(pPacket->data() + pPacket->sentSize,									\
	pPacket->length() - pPacket->sentSize), BUNDLE);														\
}																											\


#define SENDTO_BUNDLE(ENDPOINT, ADDR, PORT, BUNDLE)															\
{																											\
	EndPoint& ep = ENDPOINT;																				\
	SEND_BUNDLE_COMMON(ENDPOINT.sendto(pPacket->data() + pPacket->sentSize,									\
	pPacket->length() - pPacket->sentSize, PORT, ADDR), BUNDLE);											\
}																											\

#define MALLOC_PACKET(outputPacket, isTCPPacket)															\
{																											\
	if(isTCPPacket)																							\
		outputPacket = TCPPacket::createPoolObject();														\
	else																									\
		outputPacket = UDPPacket::createPoolObject();														\
}																											\


#define RECLAIM_PACKET(isTCPPacket, pPacket)																\
{																											\
	if(isTCPPacket)																							\
		TCPPacket::reclaimPoolObject(static_cast<TCPPacket*>(pPacket));										\
	else																									\
		UDPPacket::reclaimPoolObject(static_cast<UDPPacket*>(pPacket));										\
}																											\


// Trace packet combined with server-side configuration options to use to track a message output
#define TRACE_MESSAGE_PACKET(isrecv, pPacket, pCurrMsgHandler, length, addr, readPacketHead)				\
	if(Network::g_trace_packet > 0)																			\
	{																										\
		if(Network::g_trace_packet_use_logfile)																\
			DebugHelper::getSingleton().changeLogger("packetlogs");											\
																											\
		size_t headsize = 0;																				\
		if(pCurrMsgHandler && readPacketHead)																\
		{																									\
			headsize = NETWORK_MESSAGE_ID_SIZE;																\
			if (pCurrMsgHandler->msgLen == NETWORK_VARIABLE_MESSAGE)										\
			{																								\
				headsize += NETWORK_MESSAGE_LENGTH_SIZE;													\
				if (length >= NETWORK_MESSAGE_MAX_SIZE)														\
					headsize += NETWORK_MESSAGE_LENGTH1_SIZE;												\
			}																								\
		}																									\
																											\
		bool isprint = true;																				\
		if(pCurrMsgHandler)																					\
		{																									\
			std::vector<std::string>::iterator iter = std::find(Network::g_trace_packet_disables.begin(),	\
														Network::g_trace_packet_disables.end(),				\
															pCurrMsgHandler->name);							\
																											\
			if(iter != Network::g_trace_packet_disables.end())												\
			{																								\
				isprint = false;																			\
			}																								\
			else																							\
			{																								\
				DEBUG_MSG(fmt::format("{} {}:msgID:{}, currMsgLength:{}, addr:{}\n",						\
						((isrecv == true) ? "====>" : "<===="),												\
						pCurrMsgHandler->name.c_str(),														\
						pCurrMsgHandler->msgID,																\
						(length + headsize),																\
						addr));																				\
			}																								\
		}																									\
																											\
		if(isprint)																							\
		{																									\
																											\
			size_t rpos = pPacket->rpos();																	\
			if(headsize > 0)																				\
				pPacket->rpos(pPacket->rpos() - headsize);													\
																											\
			switch(Network::g_trace_packet)																	\
			{																								\
			case 1:																							\
				pPacket->hexlike();																			\
				break;																						\
			case 2:																							\
				pPacket->textlike();																		\
				break;																						\
			default:																						\
				pPacket->print_storage();																	\
				break;																						\
			};																								\
																											\
			pPacket->rpos(rpos);																			\
		}																									\
																											\
		if(Network::g_trace_packet_use_logfile)																\
			DebugHelper::getSingleton().changeLogger(COMPONENT_NAME_EX(g_componentType));					\
	}																										\


void destroyObjPool();

// network stats
extern uint64						g_numPacketsSent;
extern uint64						g_numPacketsReceived;
extern uint64						g_numBytesSent;
extern uint64						g_numBytesReceived;

// Receive Windows overflows
extern uint32						g_receiveWindowMessagesOverflowCritical;
extern uint32						g_intReceiveWindowMessagesOverflow;
extern uint32						g_extReceiveWindowMessagesOverflow;
extern uint32						g_intReceiveWindowBytesOverflow;
extern uint32						g_extReceiveWindowBytesOverflow;

extern uint32						g_sendWindowMessagesOverflowCritical;
extern uint32						g_intSendWindowMessagesOverflow;
extern uint32						g_extSendWindowMessagesOverflow;
extern uint32						g_intSendWindowBytesOverflow;
extern uint32						g_extSendWindowBytesOverflow;
extern uint32						g_intSentWindowBytesOverflow;
extern uint32						g_extSentWindowBytesOverflow;

bool initializeWatcher();
void finalise(void);

}
}

#endif // KBE_NETWORK_COMMON_H
