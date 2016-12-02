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

#ifndef KBE_DB_INTERFACE_REDIS_H
#define KBE_DB_INTERFACE_REDIS_H

#include "common.h"
#include "common/common.h"
#include "common/singleton.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"
#include "db_interface/db_interface.h"

#include "hiredis.h"
#if KBE_PLATFORM == PLATFORM_WIN32
#ifdef _DEBUG
#pragma comment (lib, "hiredis_d.lib")
#else
#pragma comment (lib, "hiredis.lib")
#endif
#endif

namespace KBEngine { 

/*
	Database interface
	tbl_Account_Auto_increment = uint64(1)
	tbl_Account:1 = hashes(name, password, xxx)
	tbl_Account:2 = hashes(name, password, xxx)
	tbl_Account:3 = hashes(name, password, xxx(array))

	// array of type
	tbl_Account_xxx_values:3:size = uint64(3)
	tbl_Account_xxx_values:3:1 = val
	tbl_Account_xxx_values:3:2 = val
	tbl_Account_xxx_values:3:3 = val	
*/
class DBInterfaceRedis : public DBInterface
{
public:
	DBInterfaceRedis(const char* name);
	virtual ~DBInterfaceRedis();

	static bool initInterface(DBInterface* pdbi);
	
	bool ping(redisContext* pRedisContext = NULL);
	
	void inTransaction(bool value)
	{
		KBE_ASSERT(inTransaction_ != value);
		inTransaction_ = value;
	}

	redisContext* context()				{ return pRedisContext_; }
	
	bool hasLostConnection() const		{ return hasLostConnection_; }
	void hasLostConnection( bool v )	{ hasLostConnection_ = v; }
	
	/**
		Check environment
	*/
	virtual bool checkEnvironment();
	
	/**
		Check for errors and correct the wrong content
		If you correct unsuccessful returns a failure
	*/
	virtual bool checkErrors();

	/**
		Associated with a database
	*/
	bool reattach();
	virtual bool attach(const char* databaseName = NULL);
	virtual bool detach();

	/**
		Gets all of the tables in the database
	*/
	virtual bool getTableNames( std::vector<std::string>& tableNames, const char * pattern);

	/**
		Gets the database name fields to a table
	*/
	virtual bool getTableItemNames(const char* tableName, std::vector<std::string>& itemNames);

	/**
		Query tables
	*/
	virtual bool query(const char* cmd, uint32 size, bool printlog = true, MemoryStream * result = NULL);
	bool query(const std::string& cmd, redisReply** pRedisReply, bool printlog = true);
	bool query(bool printlog, const char* format, ...);
	bool queryAppend(bool printlog, const char* format, ...);
	bool getQueryReply(redisReply **pRedisReply);
	
	void write_query_result(redisReply* pRedisReply, MemoryStream * result);
	void write_query_result_element(redisReply* pRedisReply, MemoryStream * result);
		
	/**
		Returns the interface description
	*/
	virtual const char* c_str();

	/** 
		Gets the error
	*/
	virtual const char* getstrerror();

	/** 
		For the error number
	*/
	virtual int getlasterror();

	/**
		Creates a stored entity table
	*/
	virtual EntityTable* createEntityTable(EntityTables* pEntityTables);

	/** 
		Delete entity from the database table
	*/
	virtual bool dropEntityTableFromDB(const char* tableName);
	
	/** 
		Delete entity from the database table field
	*/
	virtual bool dropEntityTableItemFromDB(const char* tableName, const char* tableItemName);

	/**
		Lock interface operations
	*/
	virtual bool lock();
	virtual bool unlock();

	void throwError();
	
	/**
		Handling exceptions
	*/
	virtual bool processException(std::exception & e);
	
protected:
	redisContext* pRedisContext_;
	bool hasLostConnection_;
	bool inTransaction_;	
};


}

#endif // KBE_DB_INTERFACE_REDIS_H
