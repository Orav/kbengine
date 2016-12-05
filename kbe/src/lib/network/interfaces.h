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

#ifndef KBE_NETWORK_INTERFACES_H
#define KBE_NETWORK_INTERFACES_H

namespace KBEngine { 
namespace Network
{
class Channel;
class MessageHandler;

/** This type of the Network interface used to receive general input messages
*/
class InputNotificationHandler
{
public:
	virtual ~InputNotificationHandler() {};
	virtual int handleInputNotification(int fd) = 0;
};

/** Network output of such interface is used to receive general news
*/
class OutputNotificationHandler
{
public:
	virtual ~OutputNotificationHandler() {};
	virtual int handleOutputNotification(int fd) = 0;
};

/** Such interfaces for receiving a network channel timeout messages
*/
class ChannelTimeOutHandler
{
public:
	virtual void onChannelTimeOut(Channel * pChannel) = 0;
};

/** This type of interface to receive an internal network channel to cancel registration
*/
class ChannelDeregisterHandler
{
public:
	virtual void onChannelDeregister(Channel * pChannel) = 0;
};

/** This type of interface is used to monitor the Network stats
*/
class NetworkStatsHandler
{
public:
	virtual void onSendMessage(const MessageHandler& msgHandler, int size) = 0;
	virtual void onRecvMessage(const MessageHandler& msgHandler, int size) = 0;
};


}
}

#endif // KBE_NETWORK_INTERFACES_H
