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


#ifndef KBE_BASEAPP_H
#define KBE_BASEAPP_H
	
// common include	
#include "base.h"
#include "proxy.h"
#include "profile.h"
#include "server/entity_app.h"
#include "server/pendingLoginmgr.h"
#include "server/forward_messagebuffer.h"
#include "network/endpoint.h"

//#define NDEBUG
// windows include	
#if KBE_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif
	
namespace KBEngine{

namespace Network{
	class Channel;
}

class Proxy;
class Backuper;
class Archiver;
class TelnetServer;
class RestoreEntityHandler;
class InitProgressHandler;

class Baseapp :	public EntityApp<Base>, 
				public Singleton<Baseapp>
{
public:
	enum TimeOutType
	{
		TIMEOUT_CHECK_STATUS = TIMEOUT_ENTITYAPP_MAX + 1,
		TIMEOUT_MAX
	};
	
	Baseapp(Network::EventDispatcher& dispatcher, 
		Network::NetworkInterface& ninterface, 
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~Baseapp();
	
	virtual bool installPyModules();
	virtual void onInstallPyModules();
	virtual bool uninstallPyModules();

	bool run();
	
	/** 
		相关处理接口 
	*/
	virtual void handleTimeout(TimerHandle handle, void * arg);
	virtual void handleGameTick();
	void handleCheckStatusTick();
	void handleBackup();
	void handleArchive();

	/** 
		初始化相关接口 
	*/
	bool initializeBegin();
	bool initializeEnd();
	void finalise();
	
	virtual bool canShutdown();
	virtual void onShutdownBegin();
	virtual void onShutdown(bool first);
	virtual void onShutdownEnd();

	virtual bool initializeWatcher();

	static PyObject* __py_quantumPassedPercent(PyObject* self, PyObject* args);
	float _getLoad() const { return getLoad(); }
	virtual void onUpdateLoad();

	virtual void onChannelDeregister(Network::Channel * pChannel);

	/**
		一个cellapp死亡
	*/
	void onCellAppDeath(Network::Channel * pChannel);

	/** 网络接口
		dbmgr告知已经启动的其他baseapp或者cellapp的地址
		当前app需要主动的去与他们建立连接
	*/
	virtual void onGetEntityAppFromDbmgr(Network::Channel* pChannel, 
							int32 uid, 
							std::string& username, 
							COMPONENT_TYPE componentType, COMPONENT_ID componentID, COMPONENT_ORDER globalorderID, COMPONENT_ORDER grouporderID,
							uint32 intaddr, uint16 intport, uint32 extaddr, uint16 extport, std::string& extaddrEx);
	
	/** 网络接口
		某个client向本app告知处于活动状态。
	*/
	void onClientActiveTick(Network::Channel* pChannel);

	/** 网络接口
		数据库中查询的自动entity加载信息返回
	*/
	void onEntityAutoLoadCBFromDBMgr(Network::Channel* pChannel, MemoryStream& s);

	/** 
		创建了一个entity回调
	*/
	virtual Base* onCreateEntity(PyObject* pyEntity, ScriptDefModule* sm, ENTITY_ID eid);

	/** 
		创建一个entity 
	*/
	static PyObject* __py_createBase(PyObject* self, PyObject* args);
	static PyObject* __py_createBaseAnywhere(PyObject* self, PyObject* args);
	static PyObject* __py_createBaseRemotely(PyObject* self, PyObject* args);
	static PyObject* __py_createBaseFromDBID(PyObject* self, PyObject* args);
	static PyObject* __py_createBaseAnywhereFromDBID(PyObject* self, PyObject* args);
	static PyObject* __py_createBaseRemotelyFromDBID(PyObject* self, PyObject* args);
	
	/**
		创建一个新的space 
	*/
	void createInNewSpace(Base* base, PyObject* pyCellappIndex);

	/**
		恢复一个space 
	*/
	void restoreSpaceInCell(Base* base);

	/** 
		在一个负载较低的baseapp上创建一个baseEntity 
	*/
	void createBaseAnywhere(const char* entityType, PyObject* params, PyObject* pyCallback);

	/** 收到baseappmgr决定将某个baseapp要求createBaseAnywhere的请求在本baseapp上执行 
		@param entityType	: entity的类别， entities.xml中的定义的。
		@param strInitData	: 这个entity被创建后应该给他初始化的一些数据， 需要使用pickle.loads解包.
		@param componentID	: 请求创建entity的baseapp的组件ID
	*/
	void onCreateBaseAnywhere(Network::Channel* pChannel, MemoryStream& s);

	/**
	baseapp 的createBaseAnywhere的回调
	*/
	void onCreateBaseAnywhereCallback(Network::Channel* pChannel, KBEngine::MemoryStream& s);
	void _onCreateBaseAnywhereCallback(Network::Channel* pChannel, CALLBACK_ID callbackID,
		std::string& entityType, ENTITY_ID eid, COMPONENT_ID componentID);

	/**
	在一个负载较低的baseapp上创建一个baseEntity
	*/
	void createBaseRemotely(const char* entityType, COMPONENT_ID componentID, PyObject* params, PyObject* pyCallback);

	/** 收到baseappmgr决定将某个baseapp要求createBaseAnywhere的请求在本baseapp上执行
	@param entityType	: entity的类别， entities.xml中的定义的。
	@param strInitData	: 这个entity被创建后应该给他初始化的一些数据， 需要使用pickle.loads解包.
	@param componentID	: 请求创建entity的baseapp的组件ID
	*/
	void onCreateBaseRemotely(Network::Channel* pChannel, MemoryStream& s);

	/**
	baseapp 的createBaseAnywhere的回调
	*/
	void onCreateBaseRemotelyCallback(Network::Channel* pChannel, KBEngine::MemoryStream& s);
	void _onCreateBaseRemotelyCallback(Network::Channel* pChannel, CALLBACK_ID callbackID,
		std::string& entityType, ENTITY_ID eid, COMPONENT_ID componentID);

	/** 
		从db获取信息创建一个entity
	*/
	void createBaseFromDBID(const char* entityType, DBID dbid, PyObject* pyCallback, const std::string& dbInterfaceName);

	/** 网络接口
		createBaseFromDBID的回调。
	*/
	void onCreateBaseFromDBIDCallback(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/** 
		从db获取信息创建一个entity
	*/
	void createBaseAnywhereFromDBID(const char* entityType, DBID dbid, PyObject* pyCallback, const std::string& dbInterfaceName);

	/** 网络接口
		createBaseAnywhereFromDBID的回调。
	*/
	// 从baseappmgr查询用于创建实体的组件id回调
	void onGetCreateBaseAnywhereFromDBIDBestBaseappID(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/** 网络接口
		createBaseAnywhereFromDBID的回调。
	*/
	// 从数据库来的回调
	void onCreateBaseAnywhereFromDBIDCallback(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	// 请求在这个进程上创建这个entity
	void createBaseAnywhereFromDBIDOtherBaseapp(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	// 创建完毕后的回调
	void onCreateBaseAnywhereFromDBIDOtherBaseappCallback(Network::Channel* pChannel, COMPONENT_ID createByBaseappID, 
							std::string entityType, ENTITY_ID createdEntityID, CALLBACK_ID callbackID, DBID dbid);
	
	/**
	从db获取信息创建一个entity
	*/
	void createBaseRemotelyFromDBID(const char* entityType, DBID dbid, COMPONENT_ID createToComponentID, 
		PyObject* pyCallback, const std::string& dbInterfaceName);

	/** 网络接口
	createBaseRemotelyFromDBID的回调。
	*/
	// 从数据库来的回调
	void onCreateBaseRemotelyFromDBIDCallback(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	// 请求在这个进程上创建这个entity
	void createBaseRemotelyFromDBIDOtherBaseapp(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	// 创建完毕后的回调
	void onCreateBaseRemotelyFromDBIDOtherBaseappCallback(Network::Channel* pChannel, COMPONENT_ID createByBaseappID,
		std::string entityType, ENTITY_ID createdEntityID, CALLBACK_ID callbackID, DBID dbid);

	/** 
		为一个baseEntity在指定的cell上创建一个cellEntity 
	*/
	void createCellEntity(EntityMailboxAbstract* createToCellMailbox, Base* base);
	
	/** 网络接口
		createCellEntity失败的回调。
	*/
	void onCreateCellFailure(Network::Channel* pChannel, ENTITY_ID entityID);

	/** 网络接口
		createCellEntity的cell实体创建成功回调。
	*/
	void onEntityGetCell(Network::Channel* pChannel, ENTITY_ID id, COMPONENT_ID componentID, SPACE_ID spaceID);

	/** 
		通知客户端创建一个proxy对应的实体 
	*/
	bool createClientProxies(Proxy* base, bool reload = false);

	/** 
		向dbmgr请求执行一个数据库命令
	*/
	static PyObject* __py_executeRawDatabaseCommand(PyObject* self, PyObject* args);
	void executeRawDatabaseCommand(const char* datas, uint32 size, PyObject* pycallback, ENTITY_ID eid, const std::string& dbInterfaceName);
	void onExecuteRawDatabaseCommandCB(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/** 网络接口
		dbmgr发送初始信息
		startID: 初始分配ENTITY_ID 段起始位置
		endID: 初始分配ENTITY_ID 段结束位置
		startGlobalOrder: 全局启动顺序 包括各种不同组件
		startGroupOrder: 组内启动顺序， 比如在所有baseapp中第几个启动。
		machineGroupOrder: 在machine中真实的组顺序, 提供底层在某些时候判断是否为第一个baseapp时使用
	*/
	void onDbmgrInitCompleted(Network::Channel* pChannel, 
		GAME_TIME gametime, ENTITY_ID startID, ENTITY_ID endID, COMPONENT_ORDER startGlobalOrder, 
		COMPONENT_ORDER startGroupOrder, const std::string& digest);

	/** 网络接口
		dbmgr广播global数据的改变
	*/
	void onBroadcastBaseAppDataChanged(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/** 网络接口
		注册将要登录的账号, 注册后则允许登录到此网关
	*/
	void registerPendingLogin(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/** 网络接口
		新用户请求登录到网关上
	*/
	void loginBaseapp(Network::Channel* pChannel, std::string& accountName, std::string& password);

	/**
		踢出一个Channel
	*/
	void kickChannel(Network::Channel* pChannel, SERVER_ERROR_CODE failedcode);

	/** 网络接口
		重新登录 快速与网关建立交互关系(前提是之前已经登录了， 
		之后断开在服务器判定该前端的Entity未超时销毁的前提下可以快速与服务器建立连接并达到操控该entity的目的)
	*/
	void reLoginBaseapp(Network::Channel* pChannel, std::string& accountName, 
		std::string& password, uint64 key, ENTITY_ID entityID);

	/**
	   登录失败
	   @failedcode: 失败返回码 NETWORK_ERR_SRV_NO_READY:服务器没有准备好, 
									NETWORK_ERR_ILLEGAL_LOGIN:非法登录, 
									NETWORK_ERR_NAME_PASSWORD:用户名或者密码不正确
	*/
	void loginBaseappFailed(Network::Channel* pChannel, std::string& accountName, 
		SERVER_ERROR_CODE failedcode, bool relogin = false);

	/** 网络接口
		从dbmgr获取到账号Entity信息
	*/
	void onQueryAccountCBFromDbmgr(Network::Channel* pChannel, KBEngine::MemoryStream& s);
	
	/**
		客户端自身进入世界了
	*/
	void onClientEntityEnterWorld(Proxy* base, COMPONENT_ID componentID);

	/** 网络接口
		entity收到一封mail, 由某个app上的mailbox发起(只限与服务器内部使用， 客户端的mailbox调用方法走
		onRemoteCellMethodCallFromClient)
	*/
	void onEntityMail(Network::Channel* pChannel, KBEngine::MemoryStream& s);
	
	/** 网络接口
		client访问entity的cell方法
	*/
	void onRemoteCallCellMethodFromClient(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/** 网络接口
		client更新数据
	*/
	void onUpdateDataFromClient(Network::Channel* pChannel, KBEngine::MemoryStream& s);
	void onUpdateDataFromClientForControlledEntity(Network::Channel* pChannel, KBEngine::MemoryStream& s);


	/** 网络接口
		cellapp备份entity的cell数据
	*/
	void onBackupEntityCellData(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/** 网络接口
		cellapp writeToDB完成
	*/
	void onCellWriteToDBCompleted(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/** 网络接口
		cellapp转发entity消息给client
	*/
	void forwardMessageToClientFromCellapp(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/** 网络接口
		cellapp转发entity消息给某个baseEntity的cellEntity
	*/
	void forwardMessageToCellappFromCellapp(Network::Channel* pChannel, KBEngine::MemoryStream& s);
	
	/**
		获取游戏时间
	*/
	static PyObject* __py_gametime(PyObject* self, PyObject* args);

	/** 网络接口
		写entity到db回调
	*/
	void onWriteToDBCallback(Network::Channel* pChannel, ENTITY_ID eid, DBID entityDBID, 
		uint16 dbInterfaceIndex, CALLBACK_ID callbackID, bool success);

	/**
		增加proxices计数
	*/
	void incProxicesCount() { ++numProxices_; }

	/**
		减少proxices计数
	*/
	void decProxicesCount() { --numProxices_; }

	/**
		获得proxices计数
	*/
	int32 numProxices() const { return numProxices_; }

	/**
		获得numClients计数
	*/
	int32 numClients() { return this->networkInterface().numExtChannels(); }
	
	/** 
		请求充值
	*/
	static PyObject* __py_charge(PyObject* self, PyObject* args);
	void charge(std::string chargeID, DBID dbid, const std::string& datas, PyObject* pycallback);
	void onChargeCB(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/**
		hook mailboxcall
	*/
	RemoteEntityMethod* createMailboxCallEntityRemoteMethod(MethodDescription* pMethodDescription, EntityMailbox* pMailbox);

	virtual void onHello(Network::Channel* pChannel, 
		const std::string& verInfo, 
		const std::string& scriptVerInfo, 
		const std::string& encryptedKey);

	// 引擎版本不匹配
	virtual void onVersionNotMatch(Network::Channel* pChannel);

	// 引擎脚本层版本不匹配
	virtual void onScriptVersionNotMatch(Network::Channel* pChannel);

	/** 网络接口
		请求在其他APP灾难恢复返回结果
	*/
	void onRequestRestoreCB(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/**
		一个cell的entity都恢复完毕
	*/
	void onRestoreEntitiesOver(RestoreEntityHandler* pRestoreEntityHandler);

	/** 网络接口
		某个baseapp上的space恢复了cell， 判断当前baseapp是否有相关entity需要恢复cell
	*/
	void onRestoreSpaceCellFromOtherBaseapp(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/** 网络接口
		某个app请求查看该app
	*/
	virtual void lookApp(Network::Channel* pChannel);

	/** 网络接口
		客户端协议导出
	*/
	void importClientMessages(Network::Channel* pChannel);

	/** 网络接口
		客户端entitydef导出
	*/
	void importClientEntityDef(Network::Channel* pChannel);

	/**
		重新导入所有的脚本
	*/
	static PyObject* __py_reloadScript(PyObject* self, PyObject* args);
	virtual void reloadScript(bool fullReload);
	virtual void onReloadScript(bool fullReload);

	/**
		获取进程是否正在关闭中
	*/
	static PyObject* __py_isShuttingDown(PyObject* self, PyObject* args);

	/**
		获取进程内部网络地址
	*/
	static PyObject* __py_address(PyObject* self, PyObject* args);

	/**
		通过dbid从数据库中删除一个实体

		从数据库删除实体， 如果实体不在线则可以直接删除回调返回true， 如果在线则回调返回的是entity的mailbox， 其他任何原因都返回false.
	*/
	static PyObject* __py_deleteBaseByDBID(PyObject* self, PyObject* args);

	/** 网络接口
		通过dbid从数据库中删除一个实体的回调
	*/
	void deleteBaseByDBIDCB(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/**
		通过dbid查询一个实体是否从数据库检出

		如果实体在线回调返回basemailbox，如果实体不在线则回调返回true，其他任何原因都返回false.
	*/
	static PyObject* __py_lookUpBaseByDBID(PyObject* self, PyObject* args);

	/** 网络接口
		如果实体在线回调返回basemailbox，如果实体不在线则回调返回true，其他任何原因都返回false.
	*/
	void lookUpBaseByDBIDCB(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/** 网络接口
		请求绑定email
	*/
	void reqAccountBindEmail(Network::Channel* pChannel, ENTITY_ID entityID, std::string& password, std::string& email);

	void onReqAccountBindEmailCB(Network::Channel* pChannel, ENTITY_ID entityID, std::string& accountName, std::string& email,
		SERVER_ERROR_CODE failedcode, std::string& code);

	/** 网络接口
		请求绑定email
	*/
	void reqAccountNewPassword(Network::Channel* pChannel, ENTITY_ID entityID, std::string& oldpassworld, std::string& newpassword);

	void onReqAccountNewPasswordCB(Network::Channel* pChannel, ENTITY_ID entityID, std::string& accountName,
		SERVER_ERROR_CODE failedcode);

	uint32 flags() const { return flags_; }
	void flags(uint32 v) { flags_ = v; }
	static PyObject* __py_setFlags(PyObject* self, PyObject* args);
	static PyObject* __py_getFlags(PyObject* self, PyObject* args);
	
protected:
	TimerHandle												loopCheckTimerHandle_;

	// globalBases
	GlobalDataClient*										pBaseAppData_;

	// 记录登录到服务器但还未处理完毕的账号
	PendingLoginMgr											pendingLoginMgr_;

	ForwardComponent_MessageBuffer							forward_messagebuffer_;

	// 备份存档相关
	KBEShared_ptr< Backuper >								pBackuper_;	
	KBEShared_ptr< Archiver >								pArchiver_;	

	int32													numProxices_;

	TelnetServer*											pTelnetServer_;

	std::vector< KBEShared_ptr< RestoreEntityHandler > >	pRestoreEntityHandlers_;

	TimerHandle												pResmgrTimerHandle_;

	InitProgressHandler*									pInitProgressHandler_;
	
	// APP的标志
	uint32													flags_;
};

}

#endif // KBE_BASEAPP_H
