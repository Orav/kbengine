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


#ifndef CLIENT_OBJECT_BASE_H
#define CLIENT_OBJECT_BASE_H

#include "event.h"
#include "script_callbacks.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"
#include "helper/script_loglevel.h"
#include "pyscript/scriptobject.h"
#include "entitydef/entities.h"
#include "entitydef/common.h"
#include "server/callbackmgr.h"
#include "server/server_errors.h"
#include "math/math.h"

namespace KBEngine{

namespace client{
class Entity;
}

class EntityMailbox;

namespace Network
{
class Channel;
}

class ClientObjectBase : public script::ScriptObject
{
	/** 
		Subclass py operations filling in a derived class 
	*/
	INSTANCE_SCRIPT_HREADER(ClientObjectBase, ScriptObject)	
public:
	ClientObjectBase(Network::NetworkInterface& ninterface, PyTypeObject* pyType = NULL);
	virtual ~ClientObjectBase();

	Network::Channel* pServerChannel() const{ return pServerChannel_; }
	void pServerChannel(Network::Channel* pChannel){ pServerChannel_ = pChannel; }

	virtual void finalise(void);
	virtual void reset(void);
	virtual void canReset(bool v){ canReset_ = v; }

	Entities<client::Entity>* pEntities() const{ return pEntities_; }

	/**
		Create an entity 
	*/
	client::Entity* createEntity(const char* entityType, PyObject* params,
		bool isInitializeScript = true, ENTITY_ID eid = 0, bool initProperty = true, 
		EntityMailbox* base = NULL, EntityMailbox* cell = NULL);

	PY_CALLBACKMGR& callbackMgr(){ return pyCallbackMgr_; }	

	/**
		By entity iD destroy a entity 
	*/
	virtual bool destroyEntity(ENTITY_ID entityID, bool callScript);

	void tickSend();
	
	virtual Network::Channel* initLoginappChannel(std::string accountName, 
		std::string passwd, std::string ip, KBEngine::uint32 port);
	virtual Network::Channel* initBaseappChannel();

	bool createAccount();
	bool login();
	
	bool loginBaseapp();
	bool reLoginBaseapp();

	int32 appID() const{ return appID_; }
	const char* name(){ return name_.c_str(); }

	ENTITY_ID entityID(){ return entityID_; }
	DBID dbid(){ return dbid_; }

	bool registerEventHandle(EventHandle* pEventHandle);
	bool deregisterEventHandle(EventHandle* pEventHandle);
	
	void fireEvent(const EventData* pEventData);
	
	EventHandler& eventHandler(){ return eventHandler_; }

	static PyObject* __pyget_pyGetEntities(PyObject *self, void *closure)
	{
		ClientObjectBase* pClientObjectBase = static_cast<ClientObjectBase*>(self);
		Py_INCREF(pClientObjectBase->pEntities());
		return pClientObjectBase->pEntities(); 
	}

	static PyObject* __pyget_pyGetID(PyObject *self, void *closure){
		
		ClientObjectBase* pClientObjectBase = static_cast<ClientObjectBase*>(self);
		return PyLong_FromLong(pClientObjectBase->appID());	
	}
	
	static PyObject* __py_callback(PyObject* self, PyObject* args);
	static PyObject* __py_cancelCallback(PyObject* self, PyObject* args);

	static PyObject* __py_getWatcher(PyObject* self, PyObject* args);
	static PyObject* __py_getWatcherDir(PyObject* self, PyObject* args);

	static PyObject* __py_disconnect(PyObject* self, PyObject* args);

	/**
		If entitiessize is less than 256
		Gets the entity through the index iD Otherwise directly access ID
	*/
	ENTITY_ID readEntityIDFromStream(MemoryStream& s);

	/**
		By the mailbox in an attempt to get an instance of a channel
	*/
	virtual Network::Channel* findChannelByMailbox(EntityMailbox& mailbox);

	/** Network interface
		Clients interact with the server for the first time, the server returns
	*/
	virtual void onHelloCB_(Network::Channel* pChannel, const std::string& verInfo,
		const std::string& scriptVerInfo, const std::string& protocolMD5, 
		const std::string& entityDefMD5, COMPONENT_TYPE componentType);

	virtual void onHelloCB(Network::Channel* pChannel, MemoryStream& s);

	/** Network interface
		And server versions do not match
	*/
	virtual void onVersionNotMatch(Network::Channel* pChannel, MemoryStream& s);
	
	/** Network interface
		And the server does not match the version of the script
	*/
	virtual void onScriptVersionNotMatch(Network::Channel* pChannel, MemoryStream& s);

	/** Network interface
		Create account success and failure callbacks

	@Failedcode: NETWORK fail return code ERR SRV NO READY: the server is not ready,
				 NETWORK ERR ACCOUNT CREATE: create failed (already exists), 
				 NETWORK SUCCESS: account created successfully
				 SERVER ERROR CODE failedcode; @ Binary data attached: additional data is binary: UInt32 length + ByteArray
	*/
	virtual void onCreateAccountResult(Network::Channel * pChannel, MemoryStream& s);

	/** Network interface
	   Login failure callback @Failedcode: NETWORK fail return code ERR SRV NO READY: the server is not ready,
										   NETWORK ERR SRV OVERLOAD: heavy server load, 
										   NETWORK ERR NAME PASSWORD: the user name or password is not correct
	*/
	virtual void onLoginFailed(Network::Channel * pChannel, MemoryStream& s);

	/** Network interface
	   Login success
	   @IP: IP address of the server
	   @Port: server port
	*/
	virtual void onLoginSuccessfully(Network::Channel * pChannel, MemoryStream& s);

	/** Network interface
	   Login failure callback @Failedcode: NETWORK fail return code ERR SRV NO READY: the server is not ready,
											NETWORK ERR ILLEGAL LOGIN: unauthorised access,
											NETWORK ERR NAME PASSWORD: the user name or password is not correct
	*/
	virtual void onLoginBaseappFailed(Network::Channel * pChannel, SERVER_ERROR_CODE failedcode);
	virtual void onReLoginBaseappFailed(Network::Channel * pChannel, SERVER_ERROR_CODE failedcode);

	/** Network interface
	   Heavy landing baseapp success
	*/
	virtual void onReLoginBaseappSuccessfully(Network::Channel * pChannel, MemoryStream& s);

	/** Network interface
		Has created a server-side and client-side agents associated Entity
	   Can also succeed at logon callback
	   @Datas: the account entity information
	*/
	virtual void onCreatedProxies(Network::Channel * pChannel, uint64 rndUUID, 
		ENTITY_ID eid, std::string& entityType);

	/** Network interface
		Entity has entered the game on the server
	*/
	virtual void onEntityEnterWorld(Network::Channel * pChannel, MemoryStream& s);

	/** Network interface
		Entity have left on the server game world
	*/
	virtual void onEntityLeaveWorld(Network::Channel * pChannel, ENTITY_ID eid);
	virtual void onEntityLeaveWorldOptimized(Network::Channel * pChannel, MemoryStream& s);

	/** Network interface
		Tell the client whether a entity destroyed, this type of entity is usually not on entity enter world
	*/
	virtual void onEntityDestroyed(Network::Channel * pChannel, ENTITY_ID eid);

	/** Network interface
		Entity has entered the space on the server
	*/
	virtual void onEntityEnterSpace(Network::Channel * pChannel, MemoryStream& s);

	/** Network interface
		Entity had left space on the server
	*/
	virtual void onEntityLeaveSpace(Network::Channel * pChannel, ENTITY_ID eid);

	/** Network interface
		Remote calling methods of the entity 
	*/
	virtual void onRemoteMethodCall(Network::Channel* pChannel, MemoryStream& s);
	virtual void onRemoteMethodCallOptimized(Network::Channel* pChannel, MemoryStream& s);
	void onRemoteMethodCall_(ENTITY_ID eid, MemoryStream& s);

	/** Network interface
	   Kicked out of the server
	*/
	virtual void onKicked(Network::Channel * pChannel, SERVER_ERROR_CODE failedcode);

	/** Network interface
		Server Update entity properties
	*/
	virtual void onUpdatePropertys(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdatePropertysOptimized(Network::Channel* pChannel, MemoryStream& s);
	void onUpdatePropertys_(ENTITY_ID eid, MemoryStream& s);

	/** Network interface
		Server sets the position of the entity and
	*/
	virtual void onSetEntityPosAndDir(Network::Channel* pChannel, MemoryStream& s);

	/** Network interface
		Server Update avatar based position and orientation
	*/
	virtual void onUpdateBasePos(Network::Channel* pChannel, float x, float y, float z);
	virtual void onUpdateBasePosXZ(Network::Channel* pChannel, float x, float z);
	virtual void onUpdateBaseDir(Network::Channel* pChannel, MemoryStream& s);

	/** Network interface
		Server updates the Volatile data
	*/
	virtual void onUpdateData(Network::Channel* pChannel, MemoryStream& s);

	virtual void onUpdateData_ypr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_yp(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_yr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_pr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_y(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_p(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_r(Network::Channel* pChannel, MemoryStream& s);

	virtual void onUpdateData_xz(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_ypr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_yp(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_yr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_pr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_y(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_p(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xz_r(Network::Channel* pChannel, MemoryStream& s);

	virtual void onUpdateData_xyz(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_ypr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_yp(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_yr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_pr(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_y(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_p(Network::Channel* pChannel, MemoryStream& s);
	virtual void onUpdateData_xyz_r(Network::Channel* pChannel, MemoryStream& s);
	
	void _updateVolatileData(ENTITY_ID entityID, float x, float y, float z, float roll, 
		float pitch, float yaw, int8 isOnGround);

	/** 
		Update the player to the server-side 
	*/
	virtual void updatePlayerToServer();

	/** Network interface
		download stream开始了 
	*/
	virtual void onStreamDataStarted(Network::Channel* pChannel, int16 id, uint32 datasize, std::string& descr);

	/** Network interface
		Receiving stream data
	*/
	virtual void onStreamDataRecv(Network::Channel* pChannel, MemoryStream& s);

	/** Network interface
		Download stream has completed 
	*/
	virtual void onStreamDataCompleted(Network::Channel* pChannel, int16 id);

	/** Network interface
		Server to tell the client: your current (cancelled) whose displacement control synchronization
	*/
	virtual void onControlEntity(Network::Channel* pChannel, int32 entityID, int8 isControlled);

	/** Network interface
		Client messages received (usually only applies to Web)
	*/
	virtual void onImportClientMessages(Network::Channel* pChannel, MemoryStream& s){}

	/** Network interface
		Entitydef received (usually only applies to Web)
	*/
	virtual void onImportClientEntityDef(Network::Channel* pChannel, MemoryStream& s){}
	
	/** Network interface
		Error code description is derived (usually only applies to Web)
	*/
	virtual void onImportServerErrorsDescr(Network::Channel* pChannel, MemoryStream& s){}

	/** Network interface
		Reset account password request returns
	*/
	virtual void onReqAccountResetPasswordCB(Network::Channel* pChannel, SERVER_ERROR_CODE failedcode){}

	/** Network interface
		Request bound mail returned
	*/
	virtual void onReqAccountBindEmailCB(Network::Channel* pChannel, SERVER_ERROR_CODE failedcode){}

	/** Network interface
		Request to modify password return
	*/
	virtual void onReqAccountNewPasswordCB(Network::Channel* pChannel, SERVER_ERROR_CODE failedcode){}

	/** 
		Get the player instance
	*/
	client::Entity* pPlayer();

	void setTargetID(ENTITY_ID id){ 
		targetID_ = id; 
		onTargetChanged();
	}
	ENTITY_ID getTargetID() const{ return targetID_; }
	virtual void onTargetChanged(){}

	ENTITY_ID getAoiEntityID(ENTITY_ID id);
	ENTITY_ID getAoiEntityIDFromStream(MemoryStream& s);
	ENTITY_ID getAoiEntityIDByAliasID(uint8 id);

	/** 
		Space-dependent operation interface 
		Server added a geometric mapping space
	*/
	virtual void addSpaceGeometryMapping(SPACE_ID spaceID, const std::string& respath);
	virtual void onAddSpaceGeometryMapping(SPACE_ID spaceID, const std::string& respath){}
	virtual void onLoadedSpaceGeometryMapping(SPACE_ID spaceID){
		isLoadedGeometry_ = true;
	}

	const std::string& getGeometryPath();
	
	virtual void initSpaceData(Network::Channel* pChannel, MemoryStream& s);
	virtual void setSpaceData(Network::Channel* pChannel, SPACE_ID spaceID, const std::string& key, const std::string& value);
	virtual void delSpaceData(Network::Channel* pChannel, SPACE_ID spaceID, const std::string& key);
	bool hasSpaceData(const std::string& key);
	const std::string& getSpaceData(const std::string& key);
	static PyObject* __py_GetSpaceData(PyObject* self, PyObject* args);
	void clearSpace(bool isAll);

	Timers & timers() { return timers_; }
	void handleTimers();

	ScriptCallbacks & scriptCallbacks() { return scriptCallbacks_; }

	void locktime(uint64 t){ locktime_ = t; }
	uint64 locktime() const{ return locktime_; }

	virtual void onServerClosed();

	uint64 rndUUID() const{ return rndUUID_; }

	Network::NetworkInterface* pNetworkInterface()const { return &networkInterface_; }

	/** Network interface
		Server heartbeat returns
	*/
	void onAppActiveTickCB(Network::Channel* pChannel);

protected:				
	int32													appID_;

	// Server-side network channel
	Network::Channel*										pServerChannel_;

	// Storing all container entity
	Entities<client::Entity>*								pEntities_;	
	std::vector<ENTITY_ID>									pEntityIDAliasIDList_;

	PY_CALLBACKMGR											pyCallbackMgr_;

	ENTITY_ID												entityID_;
	SPACE_ID												spaceID_;

	DBID													dbid_;

	std::string												ip_;
	uint16													port_;

	std::string												baseappIP_;
	uint16													baseappPort_;

	uint64													lastSentActiveTickTime_;
	uint64													lastSentUpdateDataTime_;

	bool													connectedBaseapp_;
	bool													canReset_;

	std::string												name_;
	std::string												password_;

	std::string												clientDatas_;
	std::string												serverDatas_;

	CLIENT_CTYPE											typeClient_;

	typedef std::map<ENTITY_ID, KBEShared_ptr<MemoryStream> > BUFFEREDMESSAGE;
	BUFFEREDMESSAGE											bufferedCreateEntityMessage_;

	EventHandler											eventHandler_;

	Network::NetworkInterface&								networkInterface_;

	// Current clients the choice of target
	ENTITY_ID												targetID_;

	// Load terrain data
	bool													isLoadedGeometry_;

	SPACE_DATA												spacedatas_;

	Timers													timers_;
	ScriptCallbacks											scriptCallbacks_;

	uint64													locktime_;
	
	// To visit the gateway key
	uint64													rndUUID_; 
};



}
#endif
