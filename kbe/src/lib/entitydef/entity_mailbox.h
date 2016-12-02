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


#ifndef KBE_ENTITY_CELL_BASE_CLIENT__MAILBOX_H
#define KBE_ENTITY_CELL_BASE_CLIENT__MAILBOX_H
	
#include "common/common.h"
#include "entitydef/entitymailboxabstract.h"
#include "pyscript/scriptobject.h"

	
#ifdef KBE_SERVER
#include "server/components.h"
#endif

	
namespace KBEngine{

namespace Network
{
class Channel;
}

class ScriptDefModule;
class RemoteEntityMethod;
class MethodDescription;

class EntityMailbox : public EntityMailboxAbstract
{
	/** Subclass py operations filling in a derived class */
	INSTANCE_SCRIPT_HREADER(EntityMailbox, EntityMailboxAbstract)
public:
	typedef std::tr1::function<RemoteEntityMethod* (MethodDescription* pMethodDescription, EntityMailbox* pMailbox)> MailboxCallHookFunc;
	typedef std::tr1::function<PyObject* (COMPONENT_ID componentID, ENTITY_ID& eid)> GetEntityFunc;
	typedef std::tr1::function<Network::Channel* (EntityMailbox&)> FindChannelFunc;

	EntityMailbox(ScriptDefModule* pScriptModule, const Network::Address* pAddr, COMPONENT_ID componentID, 
		ENTITY_ID eid, ENTITY_MAILBOX_TYPE type);

	~EntityMailbox();
	
	/** 
		Script requests to obtain property or method 
	*/
	PyObject* onScriptGetAttribute(PyObject* attr);						
			
	/** 
		Get object descriptions 
	*/
	PyObject* tp_repr();
	PyObject* tp_str();
	
	void c_str(char* s, size_t size);

	/** 
		Unpickle method 
	*/
	static PyObject* __unpickle__(PyObject* self, PyObject* args);

	/** 
		Is called when the script is installed 
	*/
	static void onInstallScript(PyObject* mod);

	/** 
		By entity ID try to find an instance of it
	*/
	static PyObject* tryGetEntity(COMPONENT_ID componentID, ENTITY_ID entityID);

	/** 
		Set get entity func function mailbox address 
	*/
	static void setGetEntityFunc(GetEntityFunc func){ 
		__getEntityFunc = func; 
	};

	/** 
		Find channel func function set mailbox address 
	*/
	static void setFindChannelFunc(FindChannelFunc func){ 
		__findChannelFunc = func; 
	};

	/** 
		Set the hook call func function mailbox address 
	*/
	static void setMailboxCallHookFunc(MailboxCallHookFunc* pFunc){ 
		__hookCallFuncPtr = pFunc; 
	};

	static void resetCallHooks(){
		__hookCallFuncPtr = NULL;
		__findChannelFunc = FindChannelFunc();
		__getEntityFunc = GetEntityFunc();
	}

	virtual RemoteEntityMethod* createRemoteMethod(MethodDescription* pMethodDescription);

	virtual Network::Channel* getChannel(void);

	void reload();

	typedef std::vector<EntityMailbox*> MAILBOXS;
	static MAILBOXS mailboxs;
	
private:
	// Get an entity entity function address
	static GetEntityFunc					__getEntityFunc;
	static MailboxCallHookFunc*				__hookCallFuncPtr;
	static FindChannelFunc					__findChannelFunc;
	
protected:
	std::string								scriptModuleName_;

	// The entity objects using script module
	ScriptDefModule*						pScriptModule_;	

	void _setATIdx(MAILBOXS::size_type idx) { 
		atIdx_ = idx; 
	}

	MAILBOXS::size_type	atIdx_;
};

}
#endif // KBE_ENTITY_CELL_BASE_CLIENT__MAILBOX_H
