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
#ifndef KBE_GLOBAL_DATA_SERVER_H
#define KBE_GLOBAL_DATA_SERVER_H

#include "common/common.h"
#include "helper/debug_helper.h"
	
namespace KBEngine{
namespace Network
{
	class Channel;
}

class GlobalDataServer
{
public:	
	enum DATA_TYPE
	{
		GLOBAL_DATA,
		BASEAPP_DATA,
		CELLAPP_DATA
	};

public:	
	GlobalDataServer(DATA_TYPE dataType);
	~GlobalDataServer();
			
	/** Writing data */
	bool write(Network::Channel* pChannel, COMPONENT_TYPE componentType, const std::string& key, const std::string& value);
	
	/** Delete data */
	bool del(Network::Channel* pChannel, COMPONENT_TYPE componentType, const std::string& key);	
	
	/** Add the server components that are in need of care category */
	void addConcernComponentType(COMPONENT_TYPE ct){ concernComponentTypes_.push_back(ct); }
	
	/** Broadcast data changes to the care component */
	void broadcastDataChanged(Network::Channel* pChannel, COMPONENT_TYPE componentType, const std::string& key, 
							const std::string& value, bool isDelete = false);
	
	/** A new client login */
	void onGlobalDataClientLogon(Network::Channel* client, COMPONENT_TYPE componentType);

private:
	DATA_TYPE dataType_;

	std::vector<COMPONENT_TYPE> concernComponentTypes_;						// The Global data server need to concern, components category
	typedef std::map<std::string, std::string> DATA_MAP;
	typedef DATA_MAP::iterator DATA_MAP_KEY;
	DATA_MAP dict_;
} ;

}

#endif // KBE_GLOBAL_DATA_SERVER_H
