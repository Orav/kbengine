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

#ifndef KBE_DB_INTERFACE_H
#define KBE_DB_INTERFACE_H

#include "common/common.h"
#include "common/singleton.h"
#include "helper/debug_helper.h"
#include "db_interface/entity_table.h"
#include "server/serverconfig.h"

namespace KBEngine { 

namespace thread
{
class ThreadPool;
}

class DBUtil;

/*
	Database interface
*/
class DBInterface
{
public:
	enum DB_OP_STATE
	{
		DB_OP_READ,
		DB_OP_WRITE,
	};

	friend class DBUtil;

	DBInterface(const char* name) :
	db_port_(3306),
	db_numConnections_(1),
	lastquery_()
	{
		strncpy(name_, name, MAX_NAME);
		int dbIndex = g_kbeSrvConfig.dbInterfaceName2dbInterfaceIndex(this->name());
		KBE_ASSERT(dbIndex >= 0);
		dbIndex_ = dbIndex;
	};

	virtual ~DBInterface()
	{
	};

	/**
		Check environment
	*/
	virtual bool checkEnvironment() = 0;
	
	/**
		Check for errors and correct the wrong content
		If you correct unsuccessful returns a failure
	*/
	virtual bool checkErrors() = 0;

	/**
		Associated with a database
	*/
	virtual bool attach(const char* databaseName = NULL) = 0;
	virtual bool detach() = 0;

	/**
		Gets all of the tables in the database
	*/
	virtual bool getTableNames( std::vector<std::string>& tableNames, const char * pattern) = 0;

	/**
		Gets the database name fields to a table
	*/
	virtual bool getTableItemNames(const char* tableName, std::vector<std::string>& itemNames) = 0;

	/**
		Query tables
	*/
	virtual bool query(const char* cmd, uint32 size, bool printlog = true, MemoryStream * result = NULL) = 0;
	virtual bool query(const std::string& cmd, bool printlog = true, MemoryStream * result = NULL)
	{
		return query(cmd.c_str(), (uint32)cmd.size(), printlog, result);
	}

	/**
	Returns the name of the interface
	*/
	const char* name() const { return name_; }

	/**
	Returns the index of the interface
	*/
	uint16 dbIndex() const { return dbIndex_; }

	/**
		Returns the interface description
	*/
	virtual const char* c_str() = 0;

	/** 
		Gets the error
	*/
	virtual const char* getstrerror() = 0;

	/** 
		For the error number
	*/
	virtual int getlasterror() = 0;

	/**
		Creates a stored entity table
	*/
	virtual EntityTable* createEntityTable(EntityTables* pEntityTables) = 0;

	/** 
		Delete entity from the database table
	*/
	virtual bool dropEntityTableFromDB(const char* tableName) = 0;

	/** 
		Delete entity from the database table field
	*/
	virtual bool dropEntityTableItemFromDB(const char* tableName, const char* tableItemName) = 0;

	/**
		Lock interface operations
	*/
	virtual bool lock() = 0;
	virtual bool unlock() = 0;

	/**
		Handling exceptions
	*/
	virtual bool processException(std::exception & e) = 0;

	/**
		Gets the last SQL statement for the query
	*/
	virtual const std::string& lastquery() const{ return lastquery_; }

protected:
	char name_[MAX_BUF];									// Database interface name
	char db_type_[MAX_BUF];									// Database categories
	uint32 db_port_;										// Database port
	char db_ip_[MAX_IP];									// IP address database
	char db_username_[MAX_BUF];								// Database user name
	char db_password_[MAX_BUF];								// The password for the database
	char db_name_[MAX_BUF];									// The database name
	uint16 db_numConnections_;								// Database maximum connections
	std::string lastquery_;									// Last query description
	uint16 dbIndex_;										// Database interface that corresponds to the index
};

/*
	Database operations unit
*/
class DBUtil : public Singleton<DBUtil>
{
public:
	DBUtil();
	~DBUtil();
	
	static bool initialize();
	static void finalise();
	static bool initializeWatcher();

	static bool initThread(const std::string& dbinterfaceName);
	static bool finiThread(const std::string& dbinterfaceName);

	static DBInterface* createInterface(const std::string& name, bool showinfo = true);
	static const char* accountScriptName();
	static bool initInterface(DBInterface* pdbi);

	static void handleMainTick();

	typedef KBEUnordered_map<std::string, thread::ThreadPool*> DBThreadPoolMap;
	static thread::ThreadPool* pThreadPool(const std::string& name)
	{ 
		DBThreadPoolMap::iterator iter = pThreadPoolMaps_.find(name);
		if (iter != pThreadPoolMaps_.end())
			return iter->second;

		return NULL;
	}

private:
	static DBThreadPoolMap pThreadPoolMaps_;
};

}

#endif // KBE_DB_INTERFACE_H
