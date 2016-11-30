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


#if defined(DEFINE_IN_INTERFACE)
	#undef KBE_BASEAPPMGR_INTERFACE_H
#endif


#ifndef KBE_BASEAPPMGR_INTERFACE_H
#define KBE_BASEAPPMGR_INTERFACE_H

// common include	
#if defined(BASEAPPMGR)
#include "baseappmgr.h"
#endif
#include "baseappmgr_interface_macros.h"
#include "network/interface_defs.h"
//#define NDEBUG
// windows include	
#if KBE_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif
	
namespace KBEngine{

/**
	BASEAPPMGR所有消息接口在此定义
*/
NETWORK_INTERFACE_DECLARE_BEGIN(BaseappmgrInterface)
	// 某app注册自己的接口地址到本app
	BASEAPPMGR_MESSAGE_DECLARE_ARGS11(onRegisterNewApp,									NETWORK_VARIABLE_MESSAGE,
									int32,												uid, 
									std::string,										username,
									COMPONENT_TYPE,										componentType, 
									COMPONENT_ID,										componentID, 
									COMPONENT_ORDER,									globalorderID,
									COMPONENT_ORDER,									grouporderID,
									uint32,												intaddr, 
									uint16,												intport,
									uint32,												extaddr, 
									uint16,												extport,
									std::string,										extaddrEx)

	// 某app主动请求look。
	BASEAPPMGR_MESSAGE_DECLARE_ARGS0(lookApp,											NETWORK_FIXED_MESSAGE)

	// 某个app请求查看该app负载状态。
	BASEAPPMGR_MESSAGE_DECLARE_ARGS0(queryLoad,											NETWORK_FIXED_MESSAGE)

	// 某个app向本app告知处于活动状态。
	BASEAPPMGR_MESSAGE_DECLARE_ARGS2(onAppActiveTick,									NETWORK_FIXED_MESSAGE,
									COMPONENT_TYPE,										componentType, 
									COMPONENT_ID,										componentID)

	// baseEntity请求创建在一个新的space中。
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqCreateBaseAnywhere,							NETWORK_VARIABLE_MESSAGE)

	// baseEntity请求创建在一个新的space中。
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqCreateBaseRemotely,							NETWORK_VARIABLE_MESSAGE)

	// baseEntity请求创建在一个新的space中，查询当前最好的组件ID
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqCreateBaseAnywhereFromDBIDQueryBestBaseappID, NETWORK_VARIABLE_MESSAGE)

	// baseEntity请求创建在一个新的space中。
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqCreateBaseAnywhereFromDBID,					NETWORK_VARIABLE_MESSAGE)

	// baseEntity请求创建在一个新的space中。
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqCreateBaseRemotelyFromDBID,					NETWORK_VARIABLE_MESSAGE)
	
	// 消息转发， 由某个app想通过本app将消息转发给某个app。	
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(forwardMessage,									NETWORK_VARIABLE_MESSAGE)

	// 某个app向本app告知处于活动状态。
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(registerPendingAccountToBaseapp,					NETWORK_VARIABLE_MESSAGE)

	// 获取到baseapp的地址。
	BASEAPPMGR_MESSAGE_DECLARE_ARGS4(onPendingAccountGetBaseappAddr,					NETWORK_VARIABLE_MESSAGE,
									std::string,										loginName, 
									std::string,										accountName,
									std::string,										addr,
									uint16,												port)
									
	// 一个新登录的账号获得合法登入baseapp的权利， 现在需要将账号注册给指定的baseapp
	// 使其允许在此baseapp上登录。
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(registerPendingAccountToBaseappAddr,				NETWORK_VARIABLE_MESSAGE)

	// 请求关闭服务器
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqCloseServer,									NETWORK_VARIABLE_MESSAGE)

	// 更新baseapp信息。
	BASEAPPMGR_MESSAGE_DECLARE_ARGS5(updateBaseapp,										NETWORK_FIXED_MESSAGE,
									COMPONENT_ID,										componentID,
									ENTITY_ID,											numBases,
									ENTITY_ID,											numProxices,
									float,												load,
									uint32,												flags)

	// 请求查询watcher数据
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(queryWatcher,										NETWORK_VARIABLE_MESSAGE)

	// baseapp同步自己的初始化信息
	BASEAPPMGR_MESSAGE_DECLARE_ARGS2(onBaseappInitProgress,								NETWORK_FIXED_MESSAGE,
									COMPONENT_ID,										cid,
									float,												progress)

	// 开始profile
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(startProfile,										NETWORK_VARIABLE_MESSAGE)

	// 请求强制杀死当前app
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(reqKillServer,									NETWORK_VARIABLE_MESSAGE)

	// 查询所有相关进程负载信息
	BASEAPPMGR_MESSAGE_DECLARE_STREAM(queryAppsLoads,									NETWORK_VARIABLE_MESSAGE)

	NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}
#endif
