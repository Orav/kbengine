﻿/*
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


#include "tcp_packet_receiver.h"
#ifndef CODE_INLINE
#include "tcp_packet_receiver.inl"
#endif

#include "network/address.h"
#include "network/bundle.h"
#include "network/channel.h"
#include "network/endpoint.h"
#include "network/event_dispatcher.h"
#include "network/network_interface.h"
#include "network/event_poller.h"
#include "network/error_reporter.h"

namespace KBEngine { 
namespace Network
{

//-------------------------------------------------------------------------------------
static ObjectPool<TCPPacketReceiver> _g_objPool("TCPPacketReceiver");
ObjectPool<TCPPacketReceiver>& TCPPacketReceiver::ObjPool()
{
	return _g_objPool;
}

//-------------------------------------------------------------------------------------
TCPPacketReceiver* TCPPacketReceiver::createPoolObject()
{
	return _g_objPool.createObject();
}

//-------------------------------------------------------------------------------------
void TCPPacketReceiver::reclaimPoolObject(TCPPacketReceiver* obj)
{
	_g_objPool.reclaimObject(obj);
}

//-------------------------------------------------------------------------------------
void TCPPacketReceiver::destroyObjPool()
{
	DEBUG_MSG(fmt::format("TCPPacketReceiver::destroyObjPool(): size {}.\n", 
		_g_objPool.size()));

	_g_objPool.destroy();
}

//-------------------------------------------------------------------------------------
TCPPacketReceiver::SmartPoolObjectPtr TCPPacketReceiver::createSmartPoolObj()
{
	return SmartPoolObjectPtr(new SmartPoolObject<TCPPacketReceiver>(ObjPool().createObject(), _g_objPool));
}

//-------------------------------------------------------------------------------------
TCPPacketReceiver::TCPPacketReceiver(EndPoint & endpoint,
	   NetworkInterface & networkInterface	) :
	PacketReceiver(endpoint, networkInterface)
{
}

//-------------------------------------------------------------------------------------
TCPPacketReceiver::~TCPPacketReceiver()
{
	//DEBUG_MSG("TCPPacketReceiver::~TCPPacketReceiver()\n");
}

//-------------------------------------------------------------------------------------
bool TCPPacketReceiver::processRecv(bool expectingPacket)
{
	Channel* pChannel = getChannel();
	KBE_ASSERT(pChannel != NULL);

	if(pChannel->isCondemn())
	{
		return false;
	}

	TCPPacket* pReceiveWindow = TCPPacket::createPoolObject();
	int len = pReceiveWindow->recvFromEndPoint(*pEndpoint_);

	if (len < 0)
	{
		TCPPacket::reclaimPoolObject(pReceiveWindow);

		PacketReceiver::RecvState rstate = this->checkSocketErrors(len, expectingPacket);

		if(rstate == PacketReceiver::RECV_STATE_INTERRUPT)
		{
			onGetError(pChannel);
			return false;
		}

		return rstate == PacketReceiver::RECV_STATE_CONTINUE;
	}
	else if(len == 0) // 客户端正常退出
	{
		TCPPacket::reclaimPoolObject(pReceiveWindow);
		onGetError(pChannel);
		return false;
	}
	
	Reason ret = this->processPacket(pChannel, pReceiveWindow);

	if(ret != REASON_SUCCESS)
		this->dispatcher().errorReporter().reportException(ret, pEndpoint_->addr());
	
	return true;
}

//-------------------------------------------------------------------------------------
void TCPPacketReceiver::onGetError(Channel* pChannel)
{
	pChannel->condemn();
	pChannel->networkInterface().deregisterChannel(pChannel);
	pChannel->destroy();
}

//-------------------------------------------------------------------------------------
Reason TCPPacketReceiver::processFilteredPacket(Channel* pChannel, Packet * pPacket)
{
	// 如果为None， 则可能是被过滤器过滤掉了(过滤器正在按照自己的规则组包解密)
	if(pPacket)
	{
		pChannel->addReceiveWindow(pPacket);
	}

	return REASON_SUCCESS;
}

//-------------------------------------------------------------------------------------
PacketReceiver::RecvState TCPPacketReceiver::checkSocketErrors(int len, bool expectingPacket)
{
#if KBE_PLATFORM == PLATFORM_WIN32
	DWORD wsaErr = WSAGetLastError();
#endif //def _WIN32

	if (
#if KBE_PLATFORM == PLATFORM_WIN32
		wsaErr == WSAEWOULDBLOCK && !expectingPacket// send出错大概是缓冲区满了, recv出错已经无数据可读了
#else
		errno == EAGAIN && !expectingPacket			// recv缓冲区已经无数据可读了
#endif
		)
	{
		return RECV_STATE_BREAK;
	}

#ifdef unix
	if (errno == EAGAIN ||							// 已经无数据可读了
		errno == ECONNREFUSED ||					// 连接被服务器拒绝
		errno == EHOSTUNREACH)						// 目的地址不可到达
	{
		this->dispatcher().errorReporter().reportException(
				REASON_NO_SUCH_PORT);

		return RECV_STATE_BREAK;
	}
#else
	/*
	存在的连接被远程主机强制关闭。通常原因为：远程主机上对等方应用程序突然停止运行，或远程主机重新启动，
	或远程主机在远程方套接字上使用了“强制”关闭（参见setsockopt(SO_LINGER)）。
	另外，在一个或多个操作正在进行时，如果连接因“keep-alive”活动检测到一个失败而中断，也可能导致此错误。
	此时，正在进行的操作以错误码WSAENETRESET失败返回，后续操作将失败返回错误码WSAECONNRESET
	*/
	switch(wsaErr)
	{
	case WSAECONNRESET:
		WARNING_MSG("TCPPacketReceiver::processPendingEvents: "
					"Throwing REASON_GENERAL_NETWORK - WSAECONNRESET\n");
		return RECV_STATE_INTERRUPT;
	case WSAECONNABORTED:
		WARNING_MSG("TCPPacketReceiver::processPendingEvents: "
					"Throwing REASON_GENERAL_NETWORK - WSAECONNABORTED\n");
		return RECV_STATE_INTERRUPT;
	default:
		break;

	};

#endif // unix

#if KBE_PLATFORM == PLATFORM_WIN32
	WARNING_MSG(fmt::format("TCPPacketReceiver::processPendingEvents: "
				"Throwing REASON_GENERAL_NETWORK - {}\n",
				wsaErr));
#else
	WARNING_MSG(fmt::format("TCPPacketReceiver::processPendingEvents: "
				"Throwing REASON_GENERAL_NETWORK - {}\n",
			kbe_strerror()));
#endif
	this->dispatcher().errorReporter().reportException(
			REASON_GENERAL_NETWORK);

	return RECV_STATE_CONTINUE;
}

//-------------------------------------------------------------------------------------
}
}

