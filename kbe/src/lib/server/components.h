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

#ifndef KBE_ENGINE_COMPONENT_MGR_H
#define KBE_ENGINE_COMPONENT_MGR_H
	
#include "common/timer.h"
#include "common/tasks.h"
#include "common/common.h"
#include "common/singleton.h"
#include "thread/threadmutex.h"
#include "thread/threadguard.h"
#include "server/common.h"
	
namespace KBEngine{

namespace Network
{
class Channel;
class Address;
class NetworkInterface;
}

// Component infos.flags logo
#define COMPONENT_FLAG_NORMAL 0x00000000
#define COMPONENT_FLAG_SHUTTINGDOWN 0x00000001

class Components : public Task, public Singleton<Components>
{
public:
	static int32 ANY_UID; 

	struct ComponentInfos
	{
		ComponentInfos()
		{
			uid = 0;
			flags = COMPONENT_FLAG_NORMAL;
			cid = 0;

			groupOrderid = 0;
			globalOrderid = 0;
			gus = 0;

			pChannel = NULL;
			state = COMPONENT_STATE_INIT;
			mem = cpu = 0.f;
			usedmem = 0;

			extradata = extradata1 = extradata2 = 0;

			pid = 0;
			externalAddressEx[0] = '\0';
			logTime = timestamp();
		}

		KBEShared_ptr<Network::Address> pIntAddr, pExtAddr;		// Internal and external addresses
		char externalAddressEx[MAX_NAME + 1];					// Forced exposure to external public network address, as described in configuring the external address ex

		int32 uid;
		COMPONENT_ID cid;
		COMPONENT_ORDER groupOrderid, globalOrderid;
		COMPONENT_GUS gus;
		char username[MAX_NAME + 1];
		Network::Channel* pChannel;
		COMPONENT_TYPE componentType;
		uint32 flags;

		// Process status
		COMPONENT_STATE state;

		float cpu;
		float mem;
		uint32 usedmem;
		uint64 extradata, extradata1, extradata2, extradata3;
		uint32 pid;
		uint64 logTime;
	};

	typedef std::vector<ComponentInfos> COMPONENTS;

	/** Component add remove handler */
	class ComponentsNotificationHandler
	{
	public:
		virtual ~ComponentsNotificationHandler() {};
		virtual void onAddComponent(const Components::ComponentInfos*) = 0;
		virtual void onRemoveComponent(const Components::ComponentInfos*) = 0;
		virtual void onIdentityillegal(COMPONENT_TYPE componentType, COMPONENT_ID componentID, uint32 pid, const char* pAddr) = 0;
	};

public:
	Components();
	~Components();

	void initialize(Network::NetworkInterface * pNetworkInterface, COMPONENT_TYPE componentType, COMPONENT_ID componentID);
	void finalise();

	INLINE Network::NetworkInterface* pNetworkInterface()
	{ 
		return _pNetworkInterface;
	}

	void addComponent(int32 uid, const char* username, 
		COMPONENT_TYPE componentType, COMPONENT_ID componentID, COMPONENT_ORDER globalorderid, COMPONENT_ORDER grouporderid, COMPONENT_GUS gus,
		uint32 intaddr, uint16 intport, uint32 extaddr, uint16 extport, std::string& extaddrEx, uint32 pid,
		float cpu, float mem, uint32 usedmem, uint64 extradata, uint64 extradata1, uint64 extradata2, uint64 extradata3,
		Network::Channel* pChannel = NULL);

	void delComponent(int32 uid, COMPONENT_TYPE componentType, 
		COMPONENT_ID componentID, bool ignoreComponentID = false, bool shouldShowLog = true);

	void removeComponentByChannel(Network::Channel * pChannel, bool isShutingdown = false);

	void clear(int32 uid = -1, bool shouldShowLog = true);

	Components::COMPONENTS& getComponents(COMPONENT_TYPE componentType);

	/** 
		Find component
	*/
	Components::ComponentInfos* findComponent(COMPONENT_TYPE componentType, int32 uid, COMPONENT_ID componentID);
	Components::ComponentInfos* findComponent(COMPONENT_TYPE componentType, COMPONENT_ID componentID);
	Components::ComponentInfos* findComponent(COMPONENT_ID componentID);
	Components::ComponentInfos* findComponent(Network::Channel * pChannel);
	Components::ComponentInfos* findComponent(Network::Address* pAddress);

	/** 
		By process ID will search component
	*/
	Components::ComponentInfos* findLocalComponent(uint32 pid);

	int connectComponent(COMPONENT_TYPE componentType, int32 uid, COMPONENT_ID componentID, bool printlog = true);

	typedef std::map<int32/*uid*/, COMPONENT_ORDER/*lastorder*/> ORDER_LOG;
	ORDER_LOG& getGlobalOrderLog(){ return _globalOrderLog; }
	ORDER_LOG& getBaseappGroupOrderLog(){ return _baseappGrouplOrderLog; }
	ORDER_LOG& getCellappGroupOrderLog(){ return _cellappGrouplOrderLog; }
	ORDER_LOG& getLoginappGroupOrderLog(){ return _loginappGrouplOrderLog; }
	
	/** 
		Check that all the components to prevent duplicate UUID, you should complain.
	*/
	bool checkComponents(int32 uid, COMPONENT_ID componentID, uint32 pid);

	/** 
		Sets the processor to receive notification component instance
	*/
	void pHandler(ComponentsNotificationHandler* ph){ _pHandler = ph; };

	/** 
		Check ports of a component is valid.
	*/
	bool updateComponentInfos(const Components::ComponentInfos* info);

	/** 
		Local components.
	*/
	bool isLocalComponent(const Components::ComponentInfos* info);

	/** 
		Whether in the local component is running.
	*/
	const Components::ComponentInfos* lookupLocalComponentRunning(uint32 pid);

	Components::ComponentInfos* getBaseappmgr();
	Components::ComponentInfos* getCellappmgr();
	Components::ComponentInfos* getDbmgr();
	Components::ComponentInfos* getLogger();
	Components::ComponentInfos* getInterfaceses();

	Network::Channel* getBaseappmgrChannel();
	Network::Channel* getCellappmgrChannel();
	Network::Channel* getDbmgrChannel();
	Network::Channel* getLoggerChannel();

	/**
		Statistics, the number of all components under a UID
	*/
	size_t getGameSrvComponentsSize(int32 uid);

	/** 
		Get game server registration number of necessary components。
	*/
	size_t getGameSrvComponentsSize();

	void componentID(COMPONENT_ID id){ componentID_ = id; }
	COMPONENT_ID componentID() const { return componentID_; }
	void componentType(COMPONENT_TYPE t){ componentType_ = t; }
	COMPONENT_TYPE componentType() const { return componentType_; }
	
	Network::EventDispatcher & dispatcher();

	void onChannelDeregister(Network::Channel * pChannel, bool isShutingdown);

	void extraData1(uint64 v){ extraData1_ = v; }
	void extraData2(uint64 v){ extraData2_ = v; }
	void extraData3(uint64 v){ extraData3_ = v; }
	void extraData4(uint64 v){ extraData4_ = v; }

	bool findLogger();
	
private:
	virtual bool process();
	bool findComponents();

	void onFoundAllComponents();

private:
	COMPONENTS								_baseapps;
	COMPONENTS								_cellapps;
	COMPONENTS								_dbmgrs;
	COMPONENTS								_loginapps;
	COMPONENTS								_cellappmgrs;
	COMPONENTS								_baseappmgrs;
	COMPONENTS								_machines;
	COMPONENTS								_loggers;
	COMPONENTS								_interfaceses;
	COMPONENTS								_bots;
	COMPONENTS								_consoles;

	Network::NetworkInterface*				_pNetworkInterface;
	
	// Global start order log and group component (the same components as a set, such as all baseapp group) boot order log
	// Note: died midway through the app log component here is not to do the operation reduction, judging from the intended use and there is no need to do the matching.
	ORDER_LOG								_globalOrderLog;
	ORDER_LOG								_baseappGrouplOrderLog;
	ORDER_LOG								_cellappGrouplOrderLog;
	ORDER_LOG								_loginappGrouplOrderLog;

	ComponentsNotificationHandler*			_pHandler;

	// The component category
	COMPONENT_TYPE							componentType_;

	// The component ID
	COMPONENT_ID							componentID_;	

	uint8									state_;
	int16									findIdx_;
	int8									findComponentTypes_[8];

	uint64									extraData1_;
	uint64									extraData2_;
	uint64									extraData3_;
	uint64									extraData4_;
};

}

#endif // KBE_ENGINE_COMPONENT_MGR_H
