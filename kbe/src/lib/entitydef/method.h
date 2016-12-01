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
		检查一个call是否合法 
	*/
	bool checkArgs(PyObject* args);		
	
	/** 
		将每个参数打包添加到流， 
		这个流里包含的信息是这个方法在脚本被调用时里传入的参数 
	*/
	void addToStream(MemoryStream* mstream, PyObject* args);

	/** 
		将一个call流解包 并返回一个PyObject类型的args 
	*/
	PyObject* createFromStream(MemoryStream* mstream);
	
	/** 
		呼叫一个方法 
	*/
	PyObject* call(PyObject* func, PyObject* args);	

	INLINE void currCallerID(ENTITY_ID eid);

	INLINE COMPONENT_ID domain() const;

	INLINE bool isClient() const;
	INLINE bool isCell() const;
	INLINE bool isBase() const;

	/** 
		别名id， 当暴露的方法或者广播的属性总个数小于255时
		我们不使用utype而使用1字节的aliasID来传输
	*/
	INLINE int16 aliasID() const;
	INLINE uint8 aliasIDAsUint8() const;
	INLINE void aliasID(int16 v);
	
protected:
	static uint32							methodDescriptionCount_;					// 所有的属性描述的数量

	COMPONENT_ID							methodDomain_;

	std::string								name_;										// 这个方法的名称
	ENTITY_METHOD_UID						utype_;										// 这个方法的数字类别， 用于网络上传输识别

	std::vector<DataType*>					argTypes_;									// 这个属性的参数类别列表

	bool									isExposed_;									// 是否是一个暴露方法

	ENTITY_ID								currCallerID_;								// 当前调用这个方法的调用者ID, 提供暴露方法调用时给脚本判断调用源防止作弊

	int16									aliasID_;									// 别名id， 当暴露的方法或者广播的属性总个数小于255时， 我们不使用utype而使用1字节的aliasID来传输
};

}

#ifdef CODE_INLINE
#include "method.inl"
#endif
#endif // KBENGINE_DEF_METHOD_H
