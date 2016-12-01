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

#ifndef KBE_OBJECTPOOL_H
#define KBE_OBJECTPOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>	
#include <map>	
#include <list>	
#include <vector>
#include <queue> 

#include "common/timestamp.h"
#include "thread/threadmutex.h"

namespace KBEngine{

#define OBJECT_POOL_INIT_SIZE			16
#define OBJECT_POOL_INIT_MAX_SIZE		OBJECT_POOL_INIT_SIZE * 1024

// Once every 5 minutes to slim
#define OBJECT_POOL_REDUCING_TIME_OUT	300 * stampsPerSecondD()

template< typename T >
class SmartPoolObject;

/*
	Some objects will very frequently be created, for example, Memory stream, Bundle, TCPPacket, and so on
	The object pool through server-side estimates peak effective creates a number of objects cached in advance, when used directly from a pool of objects
	Gets an object that is not in use。
*/
template< typename T, typename THREADMUTEX = KBEngine::thread::ThreadMutexNull >
class ObjectPool
{
public:
	typedef std::list<T*> OBJECTS;

	ObjectPool(std::string name):
		objects_(),
		max_(OBJECT_POOL_INIT_MAX_SIZE),
		isDestroyed_(false),
		pMutex_(new THREADMUTEX()),
		name_(name),
		total_allocs_(0),
		obj_count_(0),
		lastReducingCheckTime_(timestamp())
	{
	}

	ObjectPool(std::string name, unsigned int preAssignVal, size_t max):
		objects_(),
		max_((max == 0 ? 1 : max)),
		isDestroyed_(false),
		pMutex_(new THREADMUTEX()),
		name_(name),
		total_allocs_(0),
		obj_count_(0),
		lastReducingCheckTime_(timestamp())
	{
	}

	~ObjectPool()
	{
		destroy();
		SAFE_RELEASE(pMutex_);
	}	
	
	void destroy()
	{
		pMutex_->lockMutex();

		isDestroyed_ = true;

		typename OBJECTS::iterator iter = objects_.begin();
		for(; iter!=objects_.end(); ++iter)
		{
			(*iter)->isEnabledPoolObject(false);
			if(!(*iter)->destructorPoolObject())
			{
				delete (*iter);
			}
		}
				
		objects_.clear();	
		obj_count_ = 0;
		pMutex_->unlockMutex();
	}

	const OBJECTS& objects(void) const 
	{ 
		return objects_; 
	}

	void pMutex(KBEngine::thread::ThreadMutexNull* pMutex)
	{
		SAFE_RELEASE(pMutex_);
		pMutex_ = pMutex;
	}

	KBEngine::thread::ThreadMutexNull* pMutex()
	{
		return pMutex_;
	}

	void assignObjs(unsigned int preAssignVal = OBJECT_POOL_INIT_SIZE)
	{
		for(unsigned int i=0; i<preAssignVal; ++i)
		{
			T* t = new T();
			t->isEnabledPoolObject(false);
			objects_.push_back(t);
			++total_allocs_;
			++obj_count_;
		}
	}

	/** 
		Forces the creation of an object of the specified type. Returned if the buffer has been created in the current or
		Creates a new one, the object must be inherited from t.
	*/
	template<typename T1>
	T* createObject(void)
	{
		pMutex_->lockMutex();

		while(true)
		{
			if(obj_count_ > 0)
			{
				T* t = static_cast<T1*>(*objects_.begin());
				objects_.pop_front();
				--obj_count_;
				t->onEabledPoolObject();
				t->isEnabledPoolObject(true);
				pMutex_->unlockMutex();
				return t;
			}

			assignObjs();
		}

		pMutex_->unlockMutex();

		return NULL;
	}

	/** 
		Creates an object. Returned if the buffer has been created in the current or
		to create a new。
	*/
	T* createObject(void)
	{
		pMutex_->lockMutex();

		while(true)
		{
			if(obj_count_ > 0)
			{
				T* t = static_cast<T*>(*objects_.begin());
				objects_.pop_front();
				--obj_count_;
				t->onEabledPoolObject();
				t->isEnabledPoolObject(true);
				pMutex_->unlockMutex();
				return t;
			}

			assignObjs();
		}

		pMutex_->unlockMutex();

		return NULL;
	}

	/**
		Recovering an object
	*/
	void reclaimObject(T* obj)
	{
		pMutex_->lockMutex();
		reclaimObject_(obj);
		pMutex_->unlockMutex();
	}

	/**
		Recovering an object container
	*/
	void reclaimObject(std::list<T*>& objs)
	{
		pMutex_->lockMutex();

		typename std::list< T* >::iterator iter = objs.begin();
		for(; iter != objs.end(); ++iter)
		{
			reclaimObject_((*iter));
		}
		
		objs.clear();

		pMutex_->unlockMutex();
	}

	/**
		Recovering an object container
	*/
	void reclaimObject(std::vector< T* >& objs)
	{
		pMutex_->lockMutex();

		typename std::vector< T* >::iterator iter = objs.begin();
		for(; iter != objs.end(); ++iter)
		{
			reclaimObject_((*iter));
		}
		
		objs.clear();

		pMutex_->unlockMutex();
	}

	/**
		Recovering an object container
	*/
	void reclaimObject(std::queue<T*>& objs)
	{
		pMutex_->lockMutex();

		while(!objs.empty())
		{
			T* t = objs.front();
			objs.pop();
			reclaimObject_(t);
		}

		pMutex_->unlockMutex();
	}

	size_t size(void) const { return obj_count_; }
	
	std::string c_str()
	{
		char buf[1024];

		pMutex_->lockMutex();

		sprintf(buf, "ObjectPool::c_str(): name=%s, objs=%d/%d, isDestroyed=%s.\n", 
			name_.c_str(), (int)obj_count_, (int)max_, (isDestroyed ? "true" : "false"));

		pMutex_->unlockMutex();

		return buf;
	}

	size_t max() const { return max_; }
	size_t totalAllocs() const { return total_allocs_; }

	bool isDestroyed() const { return isDestroyed_; }

protected:
	/**
		Recovering an object
	*/
	void reclaimObject_(T* obj)
	{
		if(obj != NULL)
		{
			// Reset status
			obj->onReclaimObject();
			obj->isEnabledPoolObject(false);

			if(size() >= max_ || isDestroyed_)
			{
				delete obj;
				--total_allocs_;
			}
			else
			{
				objects_.push_back(obj);
				++obj_count_;
			}
		}

		uint64 now_timestamp = timestamp();

		if (obj_count_ <= OBJECT_POOL_INIT_SIZE)
		{
			// Less than or equal to the refresh time
			lastReducingCheckTime_ = now_timestamp;
		}
		else if (lastReducingCheckTime_ - now_timestamp > OBJECT_POOL_REDUCING_TIME_OUT)
		{
			// Long time is greater than the OBJECT POOL INIT SIZE began to clean up unused objects
			size_t reducing = std::min(objects_.size(), std::min((size_t)OBJECT_POOL_INIT_SIZE, (size_t)(obj_count_ - OBJECT_POOL_INIT_SIZE)));
			
			while (reducing-- > 0)
			{
				T* t = static_cast<T*>(*objects_.begin());
				objects_.pop_front();
				delete t;

				--obj_count_;
			}

			lastReducingCheckTime_ = now_timestamp;
		}
	}

protected:
	OBJECTS objects_;

	size_t max_;

	bool isDestroyed_;

	// Some reasons why locking is necessary
	// For example: dbMgr task output after you load navmesh log,cellapp threads in the thread callback causes the log output
	THREADMUTEX* pMutex_;

	std::string name_;

	size_t total_allocs_;

	// In a Linux environment, list.size () is used by std::distance (begin () and end ()) way to get
	// Will have an impact on performance, we own a record size
	size_t obj_count_;

	// Slim down for the last time check time
	// If the OBJECT POOL REDUCING TIME OUT is greater than the OBJECT POOL INIT-long SIZE, up to slim OBJECT POOL INIT SIZE
	uint64 lastReducingCheckTime_;
};

/*
	A pooled object, all objects must implement recovery using the pool functions.
*/
class PoolObject
{
public:
	PoolObject() : 
		isEnabledPoolObject_(false)
	{

	}

	virtual ~PoolObject(){}
	virtual void onReclaimObject() = 0;
	virtual void onEabledPoolObject() {
	}

	virtual size_t getPoolObjectBytes()
	{ 
		return 0; 
	}

	/**
		Pool object is destructed advance notice Some objects can work
	*/
	virtual bool destructorPoolObject()
	{
		return false;
	}

	bool isEnabledPoolObject() const
	{
		return isEnabledPoolObject_;
	}

	void isEnabledPoolObject(bool v)
	{
		isEnabledPoolObject_ = v;
	}

protected:

	// Pool object is active (has been removed from the pool)
	bool isEnabledPoolObject_;
};

template< typename T >
class SmartObjectPool : public ObjectPool<T>
{
public:
};

template< typename T >
class SmartPoolObject
{
public:
	SmartPoolObject(T* pPoolObject, ObjectPool<T>& objectPool):
	  pPoolObject_(pPoolObject),
	  objectPool_(objectPool)
	{
	}

	~SmartPoolObject()
	{
		onReclaimObject();
	}

	void onReclaimObject()
	{
		if(pPoolObject_ != NULL)
		{
			objectPool_.reclaimObject(pPoolObject_);
			pPoolObject_ = NULL;
		}
	}

	T* get()
	{
		return pPoolObject_;
	}

	T* operator->()
	{
		return pPoolObject_;
	}

	T& operator*()
	{
		return *pPoolObject_;
	}

private:
	T* pPoolObject_;
	ObjectPool<T>& objectPool_;
};


#define NEW_POOL_OBJECT(TYPE) TYPE::createPoolObject();


}
#endif
