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


#ifndef KBENGINE_DEF_METHOD_H
#define KBENGINE_DEF_METHOD_H

#include "common/common.h"
#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif

#include "datatype.h"
#include "datatypes.h"
#include "helper/debug_helper.h"
#include "network/packet.h"
#include "entitymailboxabstract.h"
#include "pyscript/scriptobject.h"	


namespace KBEngine{

class MethodDescription
{
public:	
	MethodDescription(ENTITY_METHOD_UID utype, COMPONENT_ID domain,
		std::string name, 
		bool isExposed = false);

	virtual ~MethodDescription();
	
	INLINE const char* getName(void) const;

	INLINE ENTITY_METHOD_UID getUType(void) const;
	INLINE void setUType(ENTITY_METHOD_UID muid);

	static uint32 getDescriptionCount(void){ return methodDescriptionCount_; }
	static void resetDescriptionCount(void){ methodDescriptionCount_ = 0; }

	INLINE bool isExposed(void) const;

	void setExposed(void);

	bool pushArgType(DataType* dataType);

	INLINE std::vector<DataType*>& getArgTypes(void);

	size_t getArgSize(void);
	
	/** 
		Check whether a call is legitimate 
	*/
	bool checkArgs(PyObject* args);		
	
	/** 
		Add to package each parameter to the stream, 
		Information contained in this stream is the method in the script when it is invoked, passing parameters 
	*/
	void addToStream(MemoryStream* mstream, PyObject* args);

	/** 
		Unpacks and returns a call flow type Py object args 
	*/
	PyObject* createFromStream(MemoryStream* mstream);
	
	/** 
		Call a method 
	*/
	PyObject* call(PyObject* func, PyObject* args);	

	INLINE void currCallerID(ENTITY_ID eid);

	INLINE COMPONENT_ID domain() const;

	INLINE bool isClient() const;
	INLINE bool isCell() const;
	INLINE bool isBase() const;

	/** 
		Alias ID, when exposed to or broadcast properties total number is less than 255
		We do not use alias iD uType uses 1 byte to transmit
	*/
	INLINE int16 aliasID() const;
	INLINE uint8 aliasIDAsUint8() const;
	INLINE void aliasID(int16 v);
	
protected:
	static uint32							methodDescriptionCount_;					// All property descriptions for quantities

	COMPONENT_ID							methodDomain_;

	std::string								name_;										// The name of this method
	ENTITY_METHOD_UID						utype_;										// This digit, used for transmission on the network to identify

	std::vector<DataType*>					argTypes_;									// This parameter category list of properties

	bool									isExposed_;									// Is an exposed method

	ENTITY_ID								currCallerID_;								// The current caller ID calls this method, called provides exposes methods to the script when it is invoked to determine the source to prevent cheating

	int16									aliasID_;									// Alias ID, when exposed to or broadcast properties total number is less than 255, 
																						// we do not use the alias iD uType uses 1 byte to transmit
};

}

#ifdef CODE_INLINE
#include "method.inl"
#endif
#endif // KBENGINE_DEF_METHOD_H
