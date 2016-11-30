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
	IDAllocate (Distributor) Used to assign a unique ID for this adapter management.
		Using the Distributor must ensure that an application can use
		
		Same ID distributor to obtain the ID is unique.
		If the type is an unsigned int, the allocator will always be assigned when the type's maximum value is reached after
		Cumulative distribution up from turned from 0, it will search from the list, if you want to assign the ID was not found in the list

This ID will be assigned.
		Usage:
		IDAllocate<ENTITY ID>* m IDAlloc ptr = new IDAllocate<ENTITY ID>;
		Assign an ID m IDAlloc PTR->alloc ()
		Recycling just one ID
		
	m IDAlloc ptr->reclaim()
		IDServer (server)
		This is to provide the entire entity between the server group iD assigned, he mainly used by baseappmgr each IDserver
		Gets the ID of the request, the server will assign a unique ID to the client, then the client can under this paragraph
		
		Generate unique ID for all and free distribution.
		Usage:
		IDServer<ENTITY ID>* m id server = new IDServer<ENTITY ID>(1, 400);
		Gets a ID and transfer to IDClient
		std::pair< unsigned int, unsigned int > id range = m id server->alloc range();
		
	g socket stream iDClient->send(id range.first, id range.second);
		IDClient (client-side)
		
		This module is combined with IDServer ID to apply for and receive.
		Usage:
		IDClient<ENTITY ID>* m id client = new IDClient<ENTITY ID>;
		Add IDServer ID sent
		m id client->on add range(id begin, id end); Assign an ID m ID client->alloc ()
*/
#ifndef KBE_IDALLOCATE_H
#define KBE_IDALLOCATE_H

#include "helper/debug_helper.h"
#include "common/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>	
#include <queue>	

	
namespace KBEngine{

class ServerApp;

// Using an iteration directly, if the number of overflows the type size will be zero so you need to use unsigned types
// Applies to the temporary allocation of ID, and return soon, so as not to ID conflict
template<typename T>
class IDAllocate
{
public:
	IDAllocate(): last_id_(0)
	{
	}

	virtual ~IDAllocate()
	{
	}	
	
	/** 
		Assign an ID 
	*/
	T alloc(void)
	{
		T t = ++last_id_;
		if(t == 0)
			t = ++last_id_;

		return t;
	}
	
	/** 
		Recycling just one ID 
	*/
	virtual void reclaim(T id)
	{
	}

	T lastID() const{ return last_id_; }
	void lastID(T v){ last_id_ = v; }

protected:
	// Last apply to ID
	T last_id_;
};

// Assigned ID run out of the store in the list, the next time you will get
template< typename T >
class IDAllocateFromList : public IDAllocate<T>
{
public:
	IDAllocateFromList(): IDAllocate<T>()
	{
	}

	~IDAllocateFromList()
	{
	}	
	
	/** 
		Assign an ID 
	*/
	T alloc(void)
	{
		if(id_list_.size() > 0)
		{
			T n = id_list_.front();
			id_list_.pop();
			return n;
		}
		
		T t = ++IDAllocate<T>::last_id_;
		if(t == 0)
			t = ++IDAllocate<T>::last_id_;

		return t;
	}
	
	/** 
		Recycling just one ID 
	*/
	void reclaim(T id)
	{
		id_list_.push(id);
	}

protected:
	// ID list, all ID there is this list
	typename std::queue< T > id_list_;
};


template< typename T >
class IDServer
{
public:
	IDServer(T id_begin, T range_step): 
	last_id_range_begin_(id_begin), 
	range_step_(range_step)
	{
	}

	~IDServer()
	{
	}
	
	/** 
		Assign an ID 
	*/
	std::pair< T, T > allocRange(void)
	{
		INFO_MSG(fmt::format("IDServer::allocRange: {}-{}.\n", 
			last_id_range_begin_, (last_id_range_begin_ + range_step_)));
		
		std::pair< T, T > p = std::make_pair(last_id_range_begin_, last_id_range_begin_ + range_step_);
		last_id_range_begin_ += range_step_;
		return p;
	}

protected:
	// Applications for the last time to the beginning of ID
	T last_id_range_begin_;
	
	// ID of a length
	T range_step_;	
};

template< typename T >
class IDClient
{										
public:
	IDClient():
	  last_id_range_begin_(0), 
	last_id_range_end_(0),
	requested_idserver_alloc_(false)
	{
	}
	
	/** 
		IDServer is not notified when the destructor for recycling, use it yourself maintenance 
	*/
	virtual ~IDClient()
	{
	}	
	
	bool hasReqServerAlloc() const { 
		return requested_idserver_alloc_; 
	}

	void setReqServerAllocFlag(bool has){ 
		requested_idserver_alloc_ = has; 
	}

	size_t size()
	{ 
		size_t nCount = last_id_range_end_ - last_id_range_begin_; 
		if(nCount <= 0)
		{
			// See if a cache ID (ID to the server when the application cache to here)
			if(id_list_.size() > 0)
			{
				std::pair< T, T > n = id_list_.front();
				last_id_range_begin_ = n.first;
				last_id_range_end_ = n.second;
				id_list_.pop();
				nCount = last_id_range_end_ - last_id_range_begin_; 
			}
		}

		return nCount;
	}
	
	/**
		Check whether the entity iD 
		Note: use a tick ID never go longer than ID ENOUGH LIMIT
	*/
	virtual void onAlloc(void) {
	};
	
	/** 
		idserver Assign an ID of 
	*/
	void onAddRange(T id_begin, T id_end)
	{
		INFO_MSG(fmt::format("IDClient::onAddRange: number of ids increased from {} to {}.\n", id_begin, id_end));
		if(size() <= 0)
		{
			last_id_range_begin_ = id_begin;
			last_id_range_end_ = id_end;
		}
		else
		{
			id_list_.push(std::make_pair(id_begin, id_end));
		}
	}
	
	/** 
		Assign an ID 
	*/
	T alloc(void)
	{
		KBE_ASSERT(size() > 0 && "IDClient:: alloc:no usable of the id.\n");

		T id = last_id_range_begin_++;

		if(last_id_range_begin_ > last_id_range_end_)
		{
			// See if a cache ID (ID to the server when the application cache to here)
			if(id_list_.size() > 0)
			{
				std::pair< T, T > n = id_list_.front();
				last_id_range_begin_ = n.first;
				last_id_range_end_ = n.second;
				id_list_.pop();
			}
			else
			{
				last_id_range_begin_ = last_id_range_end_ = 0;
			}
		}
		
		onAlloc();
		return id;
	}
	
	/** 
		Recycling just one ID
	*/
	void onReclaim(T id)
	{
	}
	
protected:
	// ID list, all ID there is this list
	typename std::queue< std::pair< T, T > > id_list_;

	// Applications for the last time to the beginning of ID
	T last_id_range_begin_;
	T last_id_range_end_;

	// Whether it has requested the ID server assigned ID
	bool requested_idserver_alloc_;	
};

class EntityIDClient : public IDClient<ENTITY_ID>
{
public:
	EntityIDClient();
	
	virtual ~EntityIDClient()
	{
	}	

	virtual void onAlloc(void);
	
	void pApp(ServerApp* pApp){ 
		pApp_ = pApp; 
	}

protected:
	ServerApp* pApp_;
};

}

#endif // KBE_IDALLOCATE_H
