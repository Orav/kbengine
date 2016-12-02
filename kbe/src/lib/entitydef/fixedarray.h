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


#ifndef _FIXED_ARRAY_TYPE_H
#define _FIXED_ARRAY_TYPE_H
#include <string>
#include "datatype.h"
#include "pyscript/sequence.h"
#include "pyscript/pickler.h"

namespace KBEngine{

class FixedArray : public script::Sequence
{		
	/** Subclass py operations filling in a derived class */
	INSTANCE_SCRIPT_HREADER(FixedArray, Sequence)
public:	
	FixedArray(DataType* dataType);
	FixedArray(DataType* dataType, std::string& strInitData);
	FixedArray(DataType* dataType, PyObject* pyInitData);
	virtual ~FixedArray();

	const DataType* getDataType(void){ return _dataType; }
	
	/** 
		Fixed array is initialized
	*/
	void initialize(std::string strInitData);
	void initialize(PyObject* pyObjInitData);

	/** 
		Supports Pickler method 
	*/
	static PyObject* __py_reduce_ex__(PyObject* self, PyObject* protocol);

	/** 
		Unpickle method 
	*/
	static PyObject* __unpickle__(PyObject* self, PyObject* args);
	
	/** 
		Is called when the script is installed 
	*/
	static void onInstallScript(PyObject* mod);
	
	/** 
		For a list of actions required interface 
	*/
	static PyObject* __py_append(PyObject* self, PyObject* args, PyObject* kwargs);	
	static PyObject* __py_count(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_extend(PyObject* self, PyObject* args, PyObject* kwargs);	
	static PyObject* __py_index(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_insert(PyObject* self, PyObject* args, PyObject* kwargs);	
	static PyObject* __py_pop(PyObject* self, PyObject* args, PyObject* kwargs);
	static PyObject* __py_remove(PyObject* self, PyObject* args, PyObject* kwargs);
	
	bool isSameType(PyObject* pyValue);
	bool isSameItemType(PyObject* pyValue);

	virtual PyObject* createNewItemFromObj(PyObject* pyItem);

	/** 
		Get object descriptions 
	*/
	PyObject* tp_repr();
	PyObject* tp_str();
protected:
	FixedArrayType* _dataType;
} ;

}
#endif
