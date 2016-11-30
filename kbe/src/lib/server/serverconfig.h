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

/*
		ServerConfig::getSingleton().loadConfig("../../res/server/KBEngine.xml");
		ENGINE_COMPONENT_INFO& ecinfo = ServerConfig::getSingleton().getCellApp();													
*/
#ifndef KBE_SERVER_CONFIG_H
#define KBE_SERVER_CONFIG_H

#define __LIB_DLLAPI__	

#include "common/common.h"
#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4996)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>	
#include <stdarg.h> 
#include "common/singleton.h"
#include "thread/threadmutex.h"
#include "thread/threadguard.h"
#include "xml/xml.h"	

	
namespace KBEngine{
namespace Network
{
class Address;
}

struct Profiles_Config
{
	Profiles_Config():
		open_pyprofile(false),
		open_cprofile(false),
		open_eventprofile(false),
		open_networkprofile(false)
	{
	}

	bool open_pyprofile;
	bool open_cprofile;
	bool open_eventprofile;
	bool open_networkprofile;
};

struct ChannelCommon
{
	float channelInternalTimeout;
	float channelExternalTimeout;
	uint32 extReadBufferSize;
	uint32 extWriteBufferSize;
	uint32 intReadBufferSize;
	uint32 intWriteBufferSize;
	uint32 intReSendInterval;
	uint32 intReSendRetries;
	uint32 extReSendInterval;
	uint32 extReSendRetries;
};

struct EmailServerInfo
{
	std::string smtp_server;
	uint32 smtp_port;
	std::string username;
	std::string password;
	uint8 smtp_auth;
};

struct EmailSendInfo
{
	std::string subject;
	std::string message;
	std::string backlink_success_message, backlink_fail_message, backlink_hello_message;

	uint32 deadline;
};

struct DBInterfaceInfo
{
	DBInterfaceInfo()
	{
		index = 0;
		isPure = false;
		db_numConnections = 5;
		db_passwordEncrypt = true;

		memset(name, 0, sizeof(name));
		memset(db_type, 0, sizeof(db_type));
		memset(db_ip, 0, sizeof(db_ip));
		memset(db_username, 0, sizeof(db_username));
		memset(db_password, 0, sizeof(db_password));
		memset(db_name, 0, sizeof(db_name));
	}

	int index;
	bool isPure;											// Is a pure library (no engine to create entity form)
	char name[MAX_BUF];										// Database interface name
	char db_type[MAX_BUF];									// Database categories
	uint32 db_port;											// Database port
	char db_ip[MAX_BUF];									// IP address database
	char db_username[MAX_NAME];								// Database user name
	char db_password[MAX_BUF * 10];							// The password for the database
	bool db_passwordEncrypt;								// DB password is encrypted
	char db_name[MAX_NAME];									// The database name
	uint16 db_numConnections;								// Database maximum connections
	std::string db_unicodeString_characterSet;				// Set database character set
	std::string db_unicodeString_collation;
};

// Engine component information structure
typedef struct EngineComponentInfo
{
	EngineComponentInfo()
	{
		tcp_SOMAXCONN = 5;
		notFoundAccountAutoCreate = false;
		account_registration_enable = false;
		account_reset_password_enable = false;
		use_coordinate_system = true;
		account_type = 3;
		debugDBMgr = false;

		externalAddress[0] = '\0';

		isOnInitCallPropertysSetMethods = true;
	}

	~EngineComponentInfo()
	{
	}

	uint32 port;											// After the component is running listening on port
	char ip[MAX_BUF];										// Component runtime IP address

	std::vector< std::string > machine_addresses;			// Configuration given all the machine address
	
	char entryScriptFile[MAX_NAME];							// Components of the entry script file
	char dbAccountEntityScriptType[MAX_NAME];				// Database account script categories
	float defaultAoIRadius;									// Configure player AOI radius size in the cellapp node
	float defaultAoIHysteresisArea;							// Configure player in the cellapp node of the lag of the AOI scope
	uint16 witness_timeout;									// Viewer default timeout (seconds)
	const Network::Address* externalAddr;					// External address
	const Network::Address* internalAddr;					// Internal address
	COMPONENT_ID componentID;

	float ghostDistance;									// Ghost zone distance
	uint16 ghostingMaxPerCheck;								// Number of checks per second ghost
	uint16 ghostUpdateHertz;								// Ghost update Hz
	
	bool use_coordinate_system;								// Using the coordinate system if it is false, AOI,trap, move and other functions will not be maintained
	bool coordinateSystem_hasY;								// Manager is managing the y axis range, note: y axis is AOI, trap and other functions with a high degree of, but management will bring a certain amount of consumption of y axis
	uint16 entity_posdir_additional_updates;				// Physical location after you stop the change, engine updates the tick location information to the client, to 0 to always update.

	bool aliasEntityID;										// Optimize Entity within the iD,aoi is less than 255 Entity iD, transfer to the client using 1 byte fake ID 
	bool entitydefAliasID;									// Optimized broadcast entity properties and methods take up bandwidth, entity client properties or when the 
															// client does not exceed 255, UID UID and property transfers to a client using 1 byte alias ID

	char internalInterface[MAX_NAME];						// Internal network adapter interface name
	char externalInterface[MAX_NAME];						// External network adapter interface name
	char externalAddress[MAX_NAME];							// External IP address
	int32 externalPorts_min;								// Foreign socket port using specified range
	int32 externalPorts_max;

	std::vector<DBInterfaceInfo> dbInterfaceInfos;			// Database interface
	bool notFoundAccountAutoCreate;							// Game database not found login lawful game account is created automatically
	bool allowEmptyDigest;									// Check defs-MD5
	bool account_registration_enable;						// If registration is open
	bool account_reset_password_enable;						// Whether or not open reset password feature

	float archivePeriod;									// Entity store database cycles
	float backupPeriod;										// Entity backup cycle
	bool backUpUndefinedProperties;							// Entity if backup does not define attribute
	uint16 entityRestoreSize;								// Entity number restore every tick 

	float loadSmoothingBias;								// Baseapp load balance adjustment filter value 
	uint32 login_port;										// Server port is making bots, using
	char login_ip[MAX_BUF];									// Server IP address

	ENTITY_ID criticallyLowSize;							// ID so many remaining dbMgr when applying for new ID resources

	uint32 downloadBitsPerSecondTotal;						// All clients per second total download bandwidth limit
	uint32 downloadBitsPerSecondPerClient;					// Each client downloads per second, bandwidth

	Profiles_Config profiles;

	uint32 defaultAddBots_totalCount;						// Default startup automatically added so multiple bots to add the total number of processes
	float defaultAddBots_tickTime;							// Automatically add the default startup process so multiple bots adding each time (s)
	uint32 defaultAddBots_tickCount;						// Default startup process number added so many bots automatically each time you add

	std::string bots_account_name_prefix;					// bots account name prefix
	uint32 bots_account_name_suffix_inc;					// bots accounts name postfix increment, 0 using random number incremented, otherwise in accordance with base NUM to fill the increasing number

	uint32 tcp_SOMAXCONN;									// Listen listen queue maximum

	int8 encrypt_login;										// Encrypted login

	uint32 telnet_port;
	std::string telnet_passwd;
	std::string telnet_deflayer;

	uint32 perSecsDestroyEntitySize;						// Number per second the destruction of base|entity

	uint64 respool_timeout;
	uint32 respool_buffersize;

	uint8 account_type;										// 1: General account, 2:email account (requires activation), 3: smart ID (automatic identification of email, ordinary number, etc) 
	uint32 accountDefaultFlags;								// New accounts default tags (ACCOUNT FLAGS can be superimposed, when filling in the decimal format) 
	uint64 accountDefaultDeadline;							// New account the default expiration time (in seconds, the engine will be added to the current time)
	
	std::string http_cbhost;
	uint16 http_cbport;										// User HTTP callback interfaces, authentication, password reset

	bool debugDBMgr;										// Reads and writes the output in debug mode information

	bool isOnInitCallPropertysSetMethods;					// Robots (bots)-specific: whether Entity initialization properties set * event is triggered
} ENGINE_COMPONENT_INFO;

class ServerConfig : public Singleton<ServerConfig>
{
public:
	ServerConfig();
	~ServerConfig();
	
	bool loadConfig(std::string fileName);
	
	INLINE ENGINE_COMPONENT_INFO& getCellApp(void);
	INLINE ENGINE_COMPONENT_INFO& getBaseApp(void);
	INLINE ENGINE_COMPONENT_INFO& getDBMgr(void);
	INLINE ENGINE_COMPONENT_INFO& getLoginApp(void);
	INLINE ENGINE_COMPONENT_INFO& getCellAppMgr(void);
	INLINE ENGINE_COMPONENT_INFO& getBaseAppMgr(void);
	INLINE ENGINE_COMPONENT_INFO& getKBMachine(void);
	INLINE ENGINE_COMPONENT_INFO& getBots(void);
	INLINE ENGINE_COMPONENT_INFO& getLogger(void);
	INLINE ENGINE_COMPONENT_INFO& getInterfaces(void);

	INLINE ENGINE_COMPONENT_INFO& getComponent(COMPONENT_TYPE componentType);
 	
	INLINE ENGINE_COMPONENT_INFO& getConfig();

 	void updateInfos(bool isPrint, COMPONENT_TYPE componentType, COMPONENT_ID componentID, 
 				const Network::Address& internalAddr, const Network::Address& externalAddr);
 	
	void updateExternalAddress(char* buf);

	INLINE int16 gameUpdateHertz(void) const;
	INLINE Network::Address interfacesAddr(void) const;

	const ChannelCommon& channelCommon(){ return channelCommon_; }

	uint32 tcp_SOMAXCONN(COMPONENT_TYPE componentType);

	float shutdowntime(){ return shutdown_time_; }
	float shutdownWaitTickTime(){ return shutdown_waitTickTime_; }

	uint32 tickMaxBufferedLogs() const { return tick_max_buffered_logs_; }
	uint32 tickMaxSyncLogs() const { return tick_max_sync_logs_; }

	INLINE bool IsPureDBInterfaceName(const std::string& dbInterfaceName);
	INLINE DBInterfaceInfo* dbInterface(const std::string& name);
	INLINE int dbInterfaceName2dbInterfaceIndex(const std::string& dbInterfaceName);
	INLINE const char* dbInterfaceIndex2dbInterfaceName(size_t dbInterfaceIndex);

private:
	void _updateEmailInfos();

private:
	ENGINE_COMPONENT_INFO _cellAppInfo;
	ENGINE_COMPONENT_INFO _baseAppInfo;
	ENGINE_COMPONENT_INFO _dbmgrInfo;
	ENGINE_COMPONENT_INFO _loginAppInfo;
	ENGINE_COMPONENT_INFO _cellAppMgrInfo;
	ENGINE_COMPONENT_INFO _baseAppMgrInfo;
	ENGINE_COMPONENT_INFO _kbMachineInfo;
	ENGINE_COMPONENT_INFO _botsInfo;
	ENGINE_COMPONENT_INFO _loggerInfo;
	ENGINE_COMPONENT_INFO _interfacesInfo;

public:
	int16 gameUpdateHertz_;
	uint32 tick_max_buffered_logs_;
	uint32 tick_max_sync_logs_;

	ChannelCommon channelCommon_;

	// Use of the maximum bandwidth per second per client
	uint32 bitsPerSecondToClient_;		

	Network::Address interfacesAddr_;
	uint32 interfaces_orders_timeout_;

	float shutdown_time_;
	float shutdown_waitTickTime_;

	float callback_timeout_;										// Default callback timeout (seconds)
	float thread_timeout_;											// The default timeout (seconds)

	uint32 thread_init_create_, thread_pre_create_, thread_max_create_;
	
	EmailServerInfo	emailServerInfo_;
	EmailSendInfo emailAtivationInfo_;
	EmailSendInfo emailResetPasswordInfo_;
	EmailSendInfo emailBindInfo_;

};

#define g_kbeSrvConfig ServerConfig::getSingleton()
}


#ifdef CODE_INLINE
#include "serverconfig.inl"
#endif
#endif // KBE_SERVER_CONFIG_H
