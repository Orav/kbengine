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
	The resource manager.
*/
#ifndef KBE_RESMGR_H
#define KBE_RESMGR_H

#include "resourceobject.h"
#include "common/common.h"
#include "common/singleton.h"
#include "common/timer.h"
#include "xml/xml.h"	
#include "common/smartpointer.h"
	
namespace KBEngine{

#define RESOURCE_NORMAL	0x00000000
#define RESOURCE_RESIDENT 0x00000001
#define RESOURCE_READ 0x00000002
#define RESOURCE_WRITE 0x00000004
#define RESOURCE_APPEND 0x00000008

class Resmgr : public Singleton<Resmgr>, public TimerHandler
{
public:
	// Engine environment variables
	struct KBEEnv
	{
		std::string root_path;
		std::string res_path;
		std::string bin_path;
	};

	static uint64 respool_timeout;
	static uint32 respool_buffersize;
	static uint32 respool_checktick;

public:
	Resmgr();
	~Resmgr();
	
	bool initialize();

	void autoSetPaths();
	void updatePaths();

	const Resmgr::KBEEnv& getEnv() { return kb_env_; }

	/*
		From the resource path (specified in the environment variable), to a complete resource address
	*/
	std::string matchRes(const std::string& res);
	std::string matchRes(const char* res);
	
	bool hasRes(const std::string& res);
	
	FILE* openRes(std::string res, const char* mode = "r");

	/*
		List down all the resource files
	*/
	bool listPathRes(std::wstring path, const std::wstring& extendName, std::vector<std::wstring>& results);

	/*
		From the resource path (environment variables specified in), to the directory
	*/
	std::string matchPath(const std::string& path);
	std::string matchPath(const char* path);

	const std::vector<std::string>& respaths() { 
		return respaths_; 
	}

	void print(void);

	bool isInit(){ 
		return isInit_; 
	}

	/**
		Access to system-level resources directory
		kbe\\res\\*
	*/
	std::string getPySysResPath();

	/**
		Access to user-level resources directory
		assets\\res\\*
	*/
	std::string getPyUserResPath();

	/**
		For user-level scripts directory
		assets\\scripts\\*
	*/
	std::string getPyUserScriptsPath();

	ResourceObjectPtr openResource(const char* res, const char* model, 
		uint32 flags = RESOURCE_NORMAL);

	bool initializeWatcher();

	void update();

private:

	virtual void handleTimeout(TimerHandle handle, void * arg);

	KBEEnv kb_env_;
	std::vector<std::string> respaths_;
	bool isInit_;

	KBEUnordered_map< std::string, ResourceObjectPtr > respool_;

	KBEngine::thread::ThreadMutex mutex_;
};

}

#endif // KBE_RESMGR_H
