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


#ifndef KBENGINE_DEF_PROPERTY_H
#define KBENGINE_DEF_PROPERTY_H

#include "common/common.h"
#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif

#include "fixedarray.h"
#include "fixeddict.h"
#include "datatype.h"
#include "common/refcountable.h"
#include "common/memorystream.h"


namespace KBEngine{

class RefCountable;
class PropertyDescription : public RefCountable
{
public:	
	PropertyDescription(ENTITY_PROPERTY_UID utype, 
		std::string dataTypeName, 
		std::string name, 
		uint32 flags, 
		bool isPersistent, 
		DataType* dataType, 
		bool isIdentifier, 
		std::string indexType,
		uint32 databaseLength, 
		std::string defaultStr, 
		DETAIL_TYPE detailLevel);

	virtual ~PropertyDescription();
	
	/** 
		Gets the property detail level 
	*/
	INLINE int8 getDetailLevel(void) const;
	
	/** 
		Whether the property is saved to the database 
	*/
	INLINE bool isPersistent(void) const;
	
	/** 
		Gets the properties of the data categories 
	*/
	INLINE DataType* getDataType(void) const;
	
	/** 
		Gets property cell, such as public 
	*/
	INLINE uint32 getFlags(void) const;
	
	/** 
		Gets the property name 
	*/
	INLINE const char* getName(void) const;
	
	/** 
		Gets the string data type UINT32, BAG..
	*/
	INLINE const char* getDataTypeName(void) const;
	
	/** 
		Gets the initial value string
	*/
	INLINE const char* getDefaultValStr(void) const;

	/** 
		Property number category, used for transmission on the network to identify 
	*/
	INLINE ENTITY_PROPERTY_UID getUType(void) const;
	
	/** 
		Gets the attribute index category
	*/
	INLINE const char* indexType(void) const;

	/** 
		Alias ID, when exposed to or broadcast properties total number is less than 255
		We do not use alias iD uType uses 1 byte to transmit
	*/
	INLINE int16 aliasID() const;
	INLINE uint8 aliasIDAsUint8() const;
	INLINE void aliasID(int16 v);

	/** 
		Setting this property to an index key 
	*/
	INLINE void setIdentifier(bool isIdentifier);
	
	/** 
		Sets the length of the attribute in the database 
	*/
	INLINE void setDatabaseLength(uint32 databaseLength);
	INLINE uint32 getDatabaseLength() const;

	/** 
		This property describes the default values defined in the def file 
	*/
	PyObject* newDefaultVal(void);
	
	/** 
		Get property description the total number of 
	*/
	static uint32 getDescriptionCount(void){ return propertyDescriptionCount_; }
	static void resetDescriptionCount(void){ propertyDescriptionCount_ = 0; }

	/** 
		Type a description instances 
	*/
	static PropertyDescription* createDescription(ENTITY_PROPERTY_UID utype, 
		std::string& dataTypeName, 
		std::string& name,
		uint32 flags, 
		bool isPersistent, 
		DataType* dataType, 
		bool isIdentifier, 
		std::string indexType,
		uint32 databaseLength,
		std::string& defaultStr, 
		DETAIL_TYPE detailLevel);
	
	/** 
		Script request to set the value of this property 
	*/
	virtual PyObject* onSetValue(PyObject* parentObj, PyObject* value);	

	virtual void addToStream(MemoryStream* mstream, PyObject* pyValue);
	virtual PyObject* createFromStream(MemoryStream* mstream);

	virtual void addPersistentToStream(MemoryStream* mstream, PyObject* pyValue);
	virtual PyObject* createFromPersistentStream(MemoryStream* mstream);

	INLINE bool hasCell(void) const;
	INLINE bool hasBase(void) const;
	INLINE bool hasClient(void) const;
	
protected:	
	static uint32				propertyDescriptionCount_;						// All property descriptions for quantities	
	std::string					name_;											// The name of the property
	std::string					dataTypeName_;									// The properties of the string data type aliases
	uint32						flags_;											// This attribute some of the flags such as cell public
	bool						isPersistent_;									// Whether the property is stored in the database
	DataType*					dataType_;										// This property of the data class
	bool						isIdentifier_;									// Whether it is a key
	uint32						databaseLength_;								// The length of the attribute in the database
	ENTITY_PROPERTY_UID			utype_;											// This property number category, used for transmission on the network to identify
	std::string					defaultValStr_;									// The default value for this property
	DETAIL_TYPE					detailLevel_;									// In common the Lod level of details of this property: property of Lod broadcast-level scope definition
	int16						aliasID_;										// Alias ID, when exposed to or broadcast properties total number is less than 255, we do not use the alias iD uType uses 1 byte to transmit
	std::string					indexType_;										// Index attributes categories, UNIQUE, INDEX, respectively, no sets, unique indexes, General indexes
};

class FixedDictDescription : public PropertyDescription
{
public:	
	FixedDictDescription(ENTITY_PROPERTY_UID utype, 
		std::string dataTypeName,
		std::string name, 
		uint32 flags, 
		bool isPersistent, 
		DataType* dataType, 
		bool isIdentifier, 
		std::string indexType,
		uint32 databaseLength, 
		std::string defaultStr, 
		DETAIL_TYPE detailLevel);

	virtual ~FixedDictDescription();
	
	/** 
		Script request to set the value of this property 
	*/
	PyObject* onSetValue(PyObject* parentObj, PyObject* value);	

	virtual void addPersistentToStream(MemoryStream* mstream, PyObject* pyValue);
	virtual PyObject* createFromPersistentStream(MemoryStream* mstream);

	typedef std::vector<std::pair<std::string, KBEShared_ptr<PropertyDescription> > > CHILD_PROPERTYS;
	
protected:
	CHILD_PROPERTYS childPropertys_;
};

class ArrayDescription : public PropertyDescription
{
public:	
	ArrayDescription(ENTITY_PROPERTY_UID utype, 
		std::string dataTypeName, 
		std::string name, 
		uint32 flags, 
		bool isPersistent, 
		DataType* dataType, 
		bool isIdentifier,
		std::string indexType,
		uint32 databaseLength, 
		std::string defaultStr, 
		DETAIL_TYPE detailLevel);

	virtual ~ArrayDescription();
	
	/** 
		Script request to set the value of this property 
	*/
	PyObject* onSetValue(PyObject* parentObj, PyObject* value);

	virtual void addPersistentToStream(MemoryStream* mstream, PyObject* pyValue);
	virtual PyObject* createFromPersistentStream(MemoryStream* mstream);
	
protected:	
};

class VectorDescription : public PropertyDescription
{
public:	
	VectorDescription(ENTITY_PROPERTY_UID utype, 
		std::string dataTypeName, 
		std::string name, 
		uint32 flags, 
		bool isPersistent, 
		DataType* dataType, 
		bool isIdentifier, 
		std::string indexType,
		uint32 databaseLength, 
		std::string defaultStr, 
		DETAIL_TYPE detailLevel, 
		uint8 elemCount);

	virtual ~VectorDescription();
	
	/** 
		Script request to set the value of this property 
	*/
	PyObject* onSetValue(PyObject* parentObj, PyObject* value);
	
protected:	
	uint8 elemCount_;
};

}

#ifdef CODE_INLINE
#include "property.inl"
#endif
#endif // KBENGINE_DEF_PROPERTY_H

