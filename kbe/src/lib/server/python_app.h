﻿/*
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

#ifndef KBE_ENTITY_APP_H
#define KBE_ENTITY_APP_H

// common include
#include "pyscript/py_gc.h"
#include "pyscript/script.h"
#include "pyscript/pyprofile.h"
#include "pyscript/pyprofile_handler.h"
#include "common/common.h"
#include "common/timer.h"
#include "common/smartpointer.h"
#include "pyscript/pyobject_pointer.h"
#include "helper/debug_helper.h"
#include "helper/script_loglevel.h"
#include "helper/profile.h"
#include "server/serverconfig.h"
#include "network/message_handler.h"
#include "resmgr/resmgr.h"
#include "helper/console_helper.h"
#include "server/serverapp.h"
#include "server/script_timers.h"

#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4996)
#endif

	
namespace KBEngine{

class PythonApp : public ServerApp
{
public:
	enum TimeOutType
	{
		TIMEOUT_GAME_TICK = TIMEOUT_SERVERAPP_MAX + 1,

		// This must be placed last, enumeration value that represents the current maximum is
		TIMEOUT_PYTHONAPP_MAX = TIMEOUT_GAME_TICK
	};

	PythonApp(Network::EventDispatcher& dispatcher,
		Network::NetworkInterface& ninterface, 
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID);

	~PythonApp();
	
	KBEngine::script::Script& getScript(){ return script_; }
	PyObjectPtr getEntryScript(){ return entryScript_; }

	int registerPyObjectToScript(const char* attrName, PyObject* pyObj);
	int unregisterPyObjectToScript(const char* attrName);

	bool installPyScript();
	virtual bool installPyModules();
	virtual void onInstallPyModules() {};
	virtual bool uninstallPyModules();
	bool uninstallPyScript();

	virtual void finalise();
	virtual bool inInitialize();
	virtual bool initializeEnd();
	virtual void onShutdownBegin();
	virtual void onShutdownEnd();

	virtual void handleTimeout(TimerHandle, void * arg);

	/** Network interface
		Requests executing a Python instructions
	*/
	void onExecScriptCommand(Network::Channel* pChannel, KBEngine::MemoryStream& s);

	/** 
		Console request profile
	*/
	virtual void startProfile_(Network::Channel* pChannel, std::string profileName, int8 profileType, uint32 timelen);

	/**
		State gets apps released, in a script, you can get the value
	*/
	static PyObject* __py_getAppPublish(PyObject* self, PyObject* args);

	/**
		Set the type of the script prefix
	*/
	static PyObject* __py_setScriptLogType(PyObject* self, PyObject* args);

	/**
		Re-import all scripts
	*/
	virtual void reloadScript(bool fullReload);
	virtual void onReloadScript(bool fullReload);

	/**
		Get resources through the relative path full path
	*/
	static PyObject* __py_getResFullPath(PyObject* self, PyObject* args);

	/**
		By relative paths to determine whether resources exist
	*/
	static PyObject* __py_hasRes(PyObject* self, PyObject* args);

	/**
		Open file
	*/
	static PyObject* __py_kbeOpen(PyObject* self, PyObject* args);

	/**
		Directory all the files that are listed
	*/
	static PyObject* __py_listPathRes(PyObject* self, PyObject* args);

	/**
		Relative full path match 
	*/
	static PyObject* __py_matchPath(PyObject* self, PyObject* args);

	/** Timer operation
	*/
	static PyObject* __py_addTimer(PyObject* self, PyObject* args);
	static PyObject* __py_delTimer(PyObject* self, PyObject* args);

	static ScriptTimers &scriptTimers() { return scriptTimers_; }


protected:
	static ScriptTimers										scriptTimers_;

	TimerHandle												gameTickTimerHandle_;

	KBEngine::script::Script								script_;

	PyObjectPtr												entryScript_;

};


}

#endif // KBE_ENTITY_APP_H
