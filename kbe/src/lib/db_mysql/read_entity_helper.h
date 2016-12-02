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

#ifndef KBE_READ_ENTITY_HELPER_H
#define KBE_READ_ENTITY_HELPER_H

// common include	
// #define NDEBUG
#include <sstream>
#include "common.h"
#include "sqlstatement.h"
#include "entity_sqlstatement_mapping.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"
#include "db_interface/db_interface.h"
#include "db_interface/entity_table.h"
#include "db_interface_mysql.h"

namespace KBEngine{ 

class ReadEntityHelper
{
public:
	ReadEntityHelper()
	{
	}

	virtual ~ReadEntityHelper()
	{
	}

	/**
		Query data from a table
	*/
	static bool queryDB(DBInterface* pdbi, mysql::DBContext& context)
	{
		// According to a dbid to obtain a list of related data
		SqlStatement* pSqlcmd = new SqlStatementQuery(pdbi, context.tableName, 
			context.dbids[context.dbid], 
			context.dbid, context.items);

		bool ret = pSqlcmd->query();
		context.dbid = pSqlcmd->dbid();
		delete pSqlcmd;
		
		if(!ret)
			return ret;

		// To write the result to a query context
		MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());

		if(pResult)
		{
			MYSQL_ROW arow;

			while((arow = mysql_fetch_row(pResult)) != NULL)
			{
				uint32 nfields = (uint32)mysql_num_fields(pResult);
				if(nfields <= 0)
					continue;

				unsigned long *lengths = mysql_fetch_lengths(pResult);

				// Query command to ensure that the query to each record will have dbid
				std::stringstream sval;
				sval << arow[0];
				DBID item_dbid;
				sval >> item_dbid;

				// The dbid records to the list, if there child table reference is to check each child table records associated with the dbid
				context.dbids[context.dbid].push_back(item_dbid);

				// If this record other data as well as dbid, you fill the data into a result set
				if(nfields > 1)
				{
					KBE_ASSERT(nfields == context.items.size() + 1);
					for (uint32 i = 1; i < nfields; ++i)
					{
						KBEShared_ptr<mysql::DBContext::DB_ITEM_DATA> pSotvs = context.items[i - 1];
						std::string data;
						data.assign(arow[i], lengths[i]);

						context.results.push_back(data);
					}
				}
			}

			mysql_free_result(pResult);
		}
		
		std::vector<DBID>& dbids = context.dbids[context.dbid];

		// If no data query end
		if(dbids.size() == 0)
			return true;

		// If the current existing child table references you need to query the table
		// Each dbid need access to child tables of data on
		// Here the dbids we make table queries all at once and fill the data into a result set

		mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
		for(; iter1 != context.optable.end(); ++iter1)
		{
			mysql::DBContext& wbox = *iter1->second.get();
			if(!queryChildDB(pdbi, wbox, dbids))
				return false;
		}

		return ret;
	}


	/**
		Query data from the child table
	*/
	static bool queryChildDB(DBInterface* pdbi, mysql::DBContext& context, std::vector<DBID>& parentTableDBIDs)
	{
		// According to a dbid to obtain a list of related data
		SqlStatement* pSqlcmd = new SqlStatementQuery(pdbi, context.tableName, 
			parentTableDBIDs, 
			context.dbid, context.items);

		bool ret = pSqlcmd->query();
		context.dbid = pSqlcmd->dbid();
		delete pSqlcmd;
		
		if(!ret)
			return ret;

		std::vector<DBID> t_parentTableDBIDs;

		// To write the result to a query context
		MYSQL_RES * pResult = mysql_store_result(static_cast<DBInterfaceMysql*>(pdbi)->mysql());

		if(pResult)
		{
			MYSQL_ROW arow;

			while((arow = mysql_fetch_row(pResult)) != NULL)
			{
				uint32 nfields = (uint32)mysql_num_fields(pResult);
				if(nfields <= 0)
					continue;

				unsigned long *lengths = mysql_fetch_lengths(pResult);

				// Query command to ensure that the query to each record will have dbid
				std::stringstream sval;
				sval << arow[0];
				DBID item_dbid;
				sval >> item_dbid;

				sval.clear();
				sval << arow[1];
				DBID parentID;
				sval >> parentID;

				// The dbid records to the list, if there child table reference is to check each child table records associated with the dbid
				context.dbids[parentID].push_back(item_dbid);
				t_parentTableDBIDs.push_back(item_dbid);

				// If this record other data as well as dbid, you fill the data into a result set
				const uint32 const_fields = 2; // id, parentID
				if(nfields > const_fields)
				{
					KBE_ASSERT(nfields == context.items.size() + const_fields);
					for (uint32 i = const_fields; i < nfields; ++i)
					{
						KBEShared_ptr<mysql::DBContext::DB_ITEM_DATA> pSotvs = context.items[i - const_fields];
						std::string data;
						data.assign(arow[i], lengths[i]);

						context.results.push_back(data);
					}
				}
			}

			mysql_free_result(pResult);
		}

		// If no data query end
		if(t_parentTableDBIDs.size() == 0)
			return true;

		// If the current existing child table references you need to query the table
		// Each dbid need access to child tables of data on
		// Here the dbids we make table queries all at once and fill the data into a result set
		mysql::DBContext::DB_RW_CONTEXTS::iterator iter1 = context.optable.begin();
		for(; iter1 != context.optable.end(); ++iter1)
		{
			mysql::DBContext& wbox = *iter1->second.get();

			if(!queryChildDB(pdbi, wbox, t_parentTableDBIDs))
				return false;
		}

		return ret;
	}

protected:
};

}
#endif // KBE_READ_ENTITY_HELPER_H
