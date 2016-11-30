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



#ifndef KBE_BASE_H
#define KBE_BASE_H
	
#include "profile.h"
#include "common/common.h"
#include "helper/debug_helper.h"
#include "pyscript/math.h"
#include "pyscript/scriptobject.h"
#include "entitydef/datatypes.h"	
#include "entitydef/entitydef.h"	
#include "entitydef/scriptdef_module.h"
#include "entitydef/entity_macro.h"	
#include "server/script_timers.h"		
	
namespace KBEngine{

class EntityMailbox;
class BaseMessagesForwardCellappHandler;
class BaseMessagesForwardClientHandler;

namespace Network
{
class Channel;
}


class Base : public script::ScriptObject
{
	/** 子类化 将一些py操作填充进派生类 */
	BASE_SCRIPT_HREADER(Base, ScriptObject)	
	ENTITY_HEADER(Base)
public:
	Base(ENTITY_ID id, const ScriptDefModule* pScriptModule, 
		PyTypeObject* pyType = getScriptType(), bool isInitialised = true);
	~Base();

	/** 
		是否存储数据库 
	*/
	INLINE bool hasDB() const;
	INLINE void hasDB(bool has);

	/** 
		数据库关联ID
	*/
	INLINE DBID dbid() const;
	INLINE void dbid(uint16 dbInterfaceIndex, DBID id);
	DECLARE_PY_GET_MOTHOD(pyGetDBID);

	/**
	数据库关联名称
	*/
	INLINE uint16 dbInterfaceIndex() const;
	DECLARE_PY_GET_MOTHOD(pyGetDBInterfaceName);

	/** 
		销毁cell部分的实体 
	*/
	bool destroyCellEntity(void);

	DECLARE_PY_MOTHOD_ARG0(pyDestroyCellEntity);
	
	/** 
		脚本请求销毁base实体 
	*/
	DECLARE_PY_MOTHOD_ARG0(pyDestroyBase);
	
	/** 
		脚本获取mailbox 
	*/
	DECLARE_PY_GET_MOTHOD(pyGetCellMailbox);

	EntityMailbox* cellMailbox(void) const;

	void cellMailbox(EntityMailbox* mailbox);
	
	/** 
		脚本获取mailbox 
	*/
	DECLARE_PY_GET_MOTHOD(pyGetClientMailbox);

	EntityMailbox* clientMailbox() const;

	void clientMailbox(EntityMailbox* mailbox);

	/**
		是否创建过space
	*/
	INLINE bool isCreatedSpace();

	/** 
		cellData部分 
	*/
	bool installCellDataAttr(PyObject* dictData = NULL, bool installpy = true);

	void createCellData(void);

	void destroyCellData(void);

	void addPersistentsDataToStream(uint32 flags, MemoryStream* s);

	PyObject* createCellDataDict(uint32 flags);

	INLINE PyObject* getCellData(void) const;
	
	INLINE bool creatingCell(void) const;

	/**
		请求cell部分将entity的celldata更新一份过来
	*/
	void reqBackupCellData();
	
	/** 
		写备份信息到流
	*/
	void writeBackupData(MemoryStream* s);
	void onBackup();

	/** 
		写存档信息到流
	*/
	void writeArchiveData(MemoryStream* s);

	/** 
		将要保存到数据库之前的通知 
	*/
	void onWriteToDB();
	void onCellWriteToDBCompleted(CALLBACK_ID callbackID, int8 shouldAutoLoad, int dbInterfaceIndex);
	void onWriteToDBCallback(ENTITY_ID eid, DBID entityDBID, uint16 dbInterfaceIndex,
		CALLBACK_ID callbackID, int8 shouldAutoLoad, bool success);

	/** 网络接口
		entity第一次写数据库由dbmgr返回的dbid
	*/
	void onGetDBID(Network::Channel* pChannel, DBID dbid);

	/** 
		创建cell失败回调 
	*/
	void onCreateCellFailure(void);

	/** 
		创建cell成功回调 
	*/
	void onGetCell(Network::Channel* pChannel, COMPONENT_ID componentID);

	/** 
		丢失cell了的通知 
	*/
	void onLoseCell(Network::Channel* pChannel, MemoryStream& s);

	/** 
		当cellapp意外终止后， baseapp如果能找到合适的cellapp则将其恢复后
		会调用此方法
	*/
	void onRestore();

	/** 
		备份cell数据
	*/
	void onBackupCellData(Network::Channel* pChannel, MemoryStream& s);

	/** 
		客户端丢失 
	*/
	void onClientDeath();

	/** 网络接口
		远程呼叫本entity的方法 
	*/
	void onRemoteMethodCall(Network::Channel* pChannel, MemoryStream& s);

	/** 
		销毁这个entity 
	*/
	void onDestroy(bool callScript);

	/**
		销毁base内部通知
	*/
	void onDestroyEntity(bool deleteFromDB, bool writeToDB);

	/** 
		为一个baseEntity在指定的cell上创建一个cellEntity 
	*/
	DECLARE_PY_MOTHOD_ARG1(createCellEntity, PyObject_ptr);
	
	/** 
		为一个baseEntity在指定的cell上还原一个cellEntity 
	*/
	void restoreCell(EntityMailboxAbstract* cellMailbox);
	INLINE bool inRestore();

	/** 
		创建一个cellEntity在一个新的space上 
	*/
	DECLARE_PY_MOTHOD_ARG1(createInNewSpace, PyObject_ptr);

	/** 网络接口
		客户端直接发送消息给cell实体
	*/
	void forwardEntityMessageToCellappFromClient(Network::Channel* pChannel, MemoryStream& s);
	
	/**
		发送消息到cellapp上
	*/
	void sendToCellapp(Network::Bundle* pBundle);
	void sendToCellapp(Network::Channel* pChannel, Network::Bundle* pBundle);

	/** 
		传送
	*/
	DECLARE_PY_MOTHOD_ARG1(pyTeleport, PyObject_ptr);

	/**
		传送回调
	*/
	void onTeleportCB(Network::Channel* pChannel, SPACE_ID spaceID, bool fromCellTeleport);  
	void onTeleportFailure();  
	void onTeleportSuccess(SPACE_ID spaceID);

	/** 网络接口
		某个entity请求teleport到这个entity的space上。
	*/
	void reqTeleportOther(Network::Channel* pChannel, ENTITY_ID reqTeleportEntityID, 
		COMPONENT_ID reqTeleportEntityCellAppID, COMPONENT_ID reqTeleportEntityBaseAppID);

	/** 网络接口
		entity请求迁移到另一个cellapp上的过程开始和结束。
	*/
	void onMigrationCellappStart(Network::Channel* pChannel, COMPONENT_ID cellappID);
	void onMigrationCellappArrived(Network::Channel* pChannel, COMPONENT_ID cellappID);
	void onMigrationCellappEnd(Network::Channel* pChannel, COMPONENT_ID cellappID);

	/**
		设置获取是否自动存档
	*/
	INLINE int8 shouldAutoArchive() const;
	INLINE void shouldAutoArchive(int8 v);
	DECLARE_PY_GETSET_MOTHOD(pyGetShouldAutoArchive, pySetShouldAutoArchive);

	/**
		设置获取是否自动备份
	*/
	INLINE int8 shouldAutoBackup() const;
	INLINE void shouldAutoBackup(int8 v);
	DECLARE_PY_GETSET_MOTHOD(pyGetShouldAutoBackup, pySetShouldAutoBackup);

	/**
		cellapp宕了
	*/
	void onCellAppDeath();

	/** 
		转发消息完成 
	*/
	void onBufferedForwardToCellappMessagesOver();
	void onBufferedForwardToClientMessagesOver();
	
	INLINE BaseMessagesForwardClientHandler* pBufferedSendToClientMessages();
	
	/** 
		设置实体持久化数据是否已脏，脏了会自动存档 
	*/
	INLINE void setDirty(bool dirty = true);
	INLINE bool isDirty() const;
	
protected:
	/** 
		定义属性数据被改变了 
	*/
	void onDefDataChanged(const PropertyDescription* propertyDescription, 
			PyObject* pyData);

	/**
		从db擦除在线log
	*/
	void eraseEntityLog();

protected:
	// 这个entity的客户端mailbox cellapp mailbox
	EntityMailbox*							clientMailbox_;
	EntityMailbox*							cellMailbox_;

	// entity创建后，在cell部分未创建时，将一些cell属性数据保存在这里
	PyObject*								cellDataDict_;

	// 是否是存储到数据库中的entity
	bool									hasDB_;
	DBID									DBID_;

	// 是否正在获取celldata中
	bool									isGetingCellData_;

	// 是否正在存档中
	bool									isArchiveing_;

	// 是否进行自动存档 <= 0为false, 1为true, KBE_NEXT_ONLY为执行一次后自动为false
	int8									shouldAutoArchive_;
	
	// 是否进行自动备份 <= 0为false, 1为true, KBE_NEXT_ONLY为执行一次后自动为false
	int8									shouldAutoBackup_;

	// 是否正在创建cell中
	bool									creatingCell_;

	// 是否已经创建了一个space
	bool									createdSpace_;

	// 是否正在恢复
	bool									inRestore_;

	// 在一些状态下(传送过程中)，发往cellapp的数据包需要被缓存, 合适的状态需要继续转发
	BaseMessagesForwardCellappHandler*		pBufferedSendToCellappMessages_;
	
	// 如果此时实体还没有被设置为ENTITY_FLAGS_TELEPORT_START,  说明onMigrationCellappArrived包优先于
	// onMigrationCellappStart到达(某些压力所致的情况下会导致实体跨进程跳转时（由cell1跳转到cell2），
	// 跳转前所产生的包会比cell2的enterSpace包慢到达)，因此发生这种情况时需要将cell2的包先缓存
	// 等cell1的包到达后执行完毕再执行cell2的包
	BaseMessagesForwardClientHandler*		pBufferedSendToClientMessages_;
	
	// 需要持久化的数据是否变脏，如果没有变脏不需要持久化
	bool									isDirty_;

	// 如果这个实体已经写到数据库，那么这个属性就是对应的数据库接口的索引
	uint16									dbInterfaceIndex_;
};

}


#ifdef CODE_INLINE
#include "base.inl"
#endif

#endif // KBE_BASE_H
