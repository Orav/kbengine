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

#ifndef KBE_REDIS_DB_RW_CONTEXT_H
#define KBE_REDIS_DB_RW_CONTEXT_H

#include "common/common.h"
#include "common/memorystream.h"
#include "helper/debug_helper.h"

namespace KBEngine { 
namespace redis { 

/**
	读Written when the delete operation is used, withdraw or to be written to contain all kinds of information.

Dbid: If you dbid is an entity the primary table, the child table is the current query dbid
		Dbids: dbids on only a dbid in the primary table, is the ID of the entity if the entity data exists an array class, the child table will appear when this data structure describes a child table
		Dbids is the child table index into an array, each dbid representing the table corresponds to the value and also said the order according to the values in the corresponding position in the array.
			dbids = {
			123: [xxx, xxx, ...],//123 a dbid that is on the parent table, array is associated with the parent table in the child table of the dbids.
		...

}

Items: the table field information is in, if it is written also wrote that corresponds to the value in the field.

Optable: table structure
	Results: query data when a read operation, arranged the data corresponds to the number of items in the strKey multiplied by the number of dbids.

Readresult idx: dbids * number of items in the results, so in certain recursive read fill the data according to the readresult calculates the filling position IDX.
	Parent table dBID: parent table dbid

Parent table name: the name of the parent table Table name: the name of the current table
 */
class DBContext
{
public:
	/**
		Table to store all the action item structure
	*/
	struct DB_ITEM_DATA
	{
		char sqlval[MAX_BUF];
		const char* sqlkey;
		std::string extraDatas;
	};

	typedef std::vector< std::pair< std::string/*tableName*/, KBEShared_ptr< DBContext > > > DB_RW_CONTEXTS;
	typedef std::vector< KBEShared_ptr<DB_ITEM_DATA>  > DB_ITEM_DATAS;

	DBContext()
	{
	}

	~DBContext()
	{
	}
	
	DB_ITEM_DATAS items;
	
	std::string tableName;
	std::string parentTableName;
	
	DBID parentTableDBID;
	DBID dbid;
	
	DB_RW_CONTEXTS optable;
	
	bool isEmpty;
	
	std::map<DBID, std::vector<DBID> > dbids;
	std::vector< std::string >results;
	std::vector< std::string >::size_type readresultIdx;

private:

};

}
}
#endif // KBE_REDIS_DB_RW_CONTEXT_H

