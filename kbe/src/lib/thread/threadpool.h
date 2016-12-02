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


#ifndef KBE_THREADPOOL_H
#define KBE_THREADPOOL_H

#include "common/common.h"
#include "common/tasks.h"
#include "helper/debug_helper.h"
#include "thread/threadtask.h"
// windows include	
#if KBE_PLATFORM == PLATFORM_WIN32
#include <windows.h>          // for HANDLE
#include <process.h>          // for _beginthread()	
#include "helper/crashhandler.h"
#else
// linux include
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>	
#endif
	
namespace KBEngine{ namespace thread{

// Thread pool activity is greater than the number of threads busy
#define THREAD_BUSY_SIZE 32

/*
	Thread pool thread Cheng Ji class
*/
class ThreadPool;
class TPThread
{
public:
	friend class ThreadPool;

	// Thread-state-1 or does not start, 0 sleep, 1 peak
	enum THREAD_STATE
	{
		THREAD_STATE_STOP = -1,
		THREAD_STATE_SLEEP = 0,
		THREAD_STATE_BUSY = 1,
		THREAD_STATE_END = 2
	};

public:
	TPThread(ThreadPool* threadPool, int threadWaitSecond = 0):
	threadWaitSecond_(threadWaitSecond), 
	currTask_(NULL), 
	threadPool_(threadPool)
	{
		state_ = THREAD_STATE_SLEEP;
		initCond();
		initMutex();
	}
		
	virtual ~TPThread()
	{
		deleteCond();
		deleteMutex();

		DEBUG_MSG(fmt::format("TPThread::~TPThread(): {}\n", (void*)this));
	}
	
	virtual void onStart(){}
	virtual void onEnd(){}

	virtual void onProcessTaskStart(TPTask* pTask) {}
	virtual void processTask(TPTask* pTask){ pTask->process(); }
	virtual void onProcessTaskEnd(TPTask* pTask) {}

	INLINE THREAD_ID id(void) const;
	
	INLINE void id(THREAD_ID tidp);
	
	/**
		Create a thread and bind themselves with this thread
	*/
	THREAD_ID createThread(void);
	
	virtual void initCond(void)
	{
		THREAD_SINGNAL_INIT(cond_);
	}

	virtual void initMutex(void)
	{
		THREAD_MUTEX_INIT(mutex_);	
	}

	virtual void deleteCond(void)
	{
		THREAD_SINGNAL_DELETE(cond_);
	}
	
	virtual void deleteMutex(void)
	{
		THREAD_MUTEX_DELETE(mutex_);
	}

	virtual void lock(void)
	{
		THREAD_MUTEX_LOCK(mutex_); 
	}
	
	virtual void unlock(void)
	{
		THREAD_MUTEX_UNLOCK(mutex_); 
	}	

	virtual TPTask* tryGetTask(void);
	
	/**
		Send signal
	*/
	int sendCondSignal(void)
	{
		return THREAD_SINGNAL_SET(cond_);
	}
	
	/**
		Thread notification wait for signal
	*/
	bool onWaitCondSignal(void);
	
	bool join(void);

	/**
		Get this thread to process tasks
	*/
	INLINE TPTask* task(void) const;

	/**
		Set the threads to process tasks
	*/
	INLINE void task(TPTask* tpt);

	INLINE int state(void) const;
	
	/**
		This thread to process a task has finished processing the tasks we decided to remove this waste
	*/
	void onTaskCompleted(void);

#if KBE_PLATFORM == PLATFORM_WIN32
	static unsigned __stdcall threadFunc(void *arg);
#else	
	static void* threadFunc(void* arg);
#endif

	/**
		Set the threads to process tasks
	*/
	INLINE ThreadPool* threadPool();

	/**
		Output thread state 
		Used primarily to Watcher
	*/
	virtual std::string printWorkState()
	{
		char buf[128];
		lock();
		sprintf(buf, "%p,%u", currTask_, done_tasks_);
		unlock();
		return buf;
	}

	/**
		Thread start time without change to idle state of continuous performance task count
	*/
	void reset_done_tasks(){ done_tasks_ = 0; }
	void inc_done_tasks(){ ++done_tasks_; }

protected:
	THREAD_SINGNAL cond_;			// Thread semaphores
	THREAD_MUTEX mutex_;			// Thread each other
	int threadWaitSecond_;			// Thread is idle for more than the number of seconds the thread exits, less than 0 for permanent threads (second unit)
	TPTask * currTask_;				// The thread currently executing tasks
	THREAD_ID tidp_;				// This thread's ID
	ThreadPool* threadPool_;		// Thread pool pointer
	THREAD_STATE state_;			// Thread States:-1 has not yet started, 0 sleep, 1 peak
	uint32 done_tasks_;				// Thread start time without change to idle state of continuous performance task count
};


class ThreadPool
{
public:		
	
	ThreadPool();
	virtual ~ThreadPool();
	
	void finalise();

	virtual void onMainThreadTick();
	
	bool hasThread(TPThread* pTPThread);

	/**
		Gets the current thread pool threads (for watch)
	*/
	std::string printThreadWorks();

	/**
		Gets the total number of current threads
	*/	
	INLINE uint32 currentThreadCount(void) const;
	
	/**
		Gets the current total number of idle threads
	*/		
	INLINE uint32 currentFreeThreadCount(void) const;
	
	/**
		Create a thread pool
		@param inewThreadCount			: When the system is busy, the thread pool will add so many new threads (temporary)
		@param inormalMaxThreadCount	: Thread pools are kept so many number of threads
		@param imaxThreadCount			: The thread pool can only be so many threads
	*/
	bool createThreadPool(uint32 inewThreadCount, 
			uint32 inormalMaxThreadCount, uint32 imaxThreadCount);
	
	/**
		To add a thread pool tasks
	*/		
	bool addTask(TPTask* tptask);
	bool _addTask(TPTask* tptask);
	INLINE bool addBackgroundTask(TPTask* tptask){ return addTask(tptask); }
	INLINE bool pushTask(TPTask* tptask){ return addTask(tptask); }

	/**
		Number reaches the maximum number of threads
	*/
	INLINE bool isThreadCountMax(void) const;
	
	/**
		The thread pool is busy Whether the task is not processed very much threads busy
	*/
	INLINE bool isBusy(void) const;
	
	/** 
		The thread pool has already been initialized 
	*/
	INLINE bool isInitialize(void) const;

	/**
		Return has been destroyed
	*/
	INLINE bool isDestroyed() const;

	/**
		Return has been destroyed
	*/
	INLINE void destroy();

	/** 
		Access to cache the number of tasks
	*/
	INLINE uint32 bufferTaskSize() const;

	/** 
		Get cache task
	*/
	INLINE std::queue<thread::TPTask*>& bufferedTaskList();

	/** 
		Action caching task lock
	*/
	INLINE void lockBufferedTaskList();
	INLINE void unlockBufferedTaskList();

	/** 
		Get number of tasks has been completed
	*/
	INLINE uint32 finiTaskSize() const;

	virtual std::string name() const { return "ThreadPool"; }

public:
	static int timeout;

	/**
		Create a thread pool thread
	*/
	virtual TPThread* createThread(int threadWaitSecond = ThreadPool::timeout, bool threadStartsImmediately = true);

	/**
		To save a task to the list of pending
	*/
	void bufferTask(TPTask* tptask);

	/**
		Never remove a task on the list and deleted from the list
	*/
	TPTask* popbufferTask(void);

	/**
		Move a thread to the free list
	*/
	bool addFreeThread(TPThread* tptd);
	
	/**
		Move a thread to a busy list
	*/	
	bool addBusyThread(TPThread* tptd);
	
	/**
		添加一个已经完成的任务到列表
	*/	
	void addFiniTask(TPTask* tptask);
	
	/**
		Delete a pending (timeout) thread
	*/	
	bool removeHangThread(TPThread* tptd);

	bool initializeWatcher();

protected:
	bool isInitialize_;												// Thread pool are initialized
	
	std::queue<TPTask*> bufferedTaskList_;							// When the system is busy or does not process the task list
	std::list<TPTask*> finiTaskList_;								// List of tasks has been completed
	size_t finiTaskList_count_;

	THREAD_MUTEX bufferedTaskList_mutex_;							// Buffer task list mutex
	THREAD_MUTEX threadStateList_mutex_;							// Processing bufferTaskList and freeThreadList _Mutual exclusion lock
	THREAD_MUTEX finiTaskList_mutex_;								// Fini task list mutex
	
	std::list<TPThread*> busyThreadList_;							// List of busy threads
	std::list<TPThread*> freeThreadList_;							// List of idle threads
	std::list<TPThread*> allThreadList_;							// List all threads

	uint32 maxThreadCount_;											// Maximum total number of threads
	uint32 extraNewAddThreadCount_;									// If the normal thread count is not sufficient to use the newly created so many threads
	uint32 currentThreadCount_;										// Current number of threads
	uint32 currentFreeThreadCount_;									// Number of threads currently idle
	uint32 normalThreadCount_;										// Under standard conditions the total number of threads that is: starting the server by default opens so many threads
																	// If the thread is not sufficient, it will create some new threads, Max Max thread NUM.

	bool isDestroyed_;
};

}


}

#ifdef CODE_INLINE
#include "threadpool.inl"
#endif
#endif // KBE_THREADPOOL_H
