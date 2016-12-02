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


#ifndef KBENGINE_DEF_COMMON_H
#define KBENGINE_DEF_COMMON_H

#include "common/common.h"
#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif


namespace KBEngine{

/** Data transmission entity is marked */
enum EntityDataFlags
{
	ED_FLAG_UNKOWN													= 0x00000000, // Is not defined
	ED_FLAG_CELL_PUBLIC												= 0x00000001, // All the cell broadcast
	ED_FLAG_CELL_PRIVATE											= 0x00000002, // The current cell
	ED_FLAG_ALL_CLIENTS												= 0x00000004, // Cell broadcast and all clients
	ED_FLAG_CELL_PUBLIC_AND_OWN										= 0x00000008, // Cell broadcast and your own client
	ED_FLAG_OWN_CLIENT												= 0x00000010, // The current cell and the client
	ED_FLAG_BASE_AND_CLIENT											= 0x00000020, // Base and client
	ED_FLAG_BASE													= 0x00000040, // The current base
	ED_FLAG_OTHER_CLIENTS											= 0x00000080, // Cell broadcast and other clients
};

std::string entityDataFlagsToString(uint32 flags);

#define ED_FLAG_ALL  ED_FLAG_CELL_PUBLIC | ED_FLAG_CELL_PRIVATE | ED_FLAG_ALL_CLIENTS \
	| ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OWN_CLIENT |	\
	ED_FLAG_BASE_AND_CLIENT | ED_FLAG_BASE | ED_FLAG_OTHER_CLIENTS

/** Is equivalent to an overall definition of entity data type */
enum EntityDataFlagRelation
{
	// All connected with baseapp mark
	ENTITY_BASE_DATA_FLAGS											= ED_FLAG_BASE | ED_FLAG_BASE_AND_CLIENT,
	// All links with the cellapp mark
	ENTITY_CELL_DATA_FLAGS											= ED_FLAG_CELL_PUBLIC | ED_FLAG_CELL_PRIVATE | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS | ED_FLAG_OWN_CLIENT,
	// All sign of the relationship with the client
	ENTITY_CLIENT_DATA_FLAGS										= ED_FLAG_BASE_AND_CLIENT | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS | ED_FLAG_OWN_CLIENT,
	// All need to be broadcast to other cellapp mark
	ENTITY_BROADCAST_CELL_FLAGS										= ED_FLAG_CELL_PUBLIC | ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OTHER_CLIENTS,
	// All need to be broadcast to other clients (not including their own) logo
	ENTITY_BROADCAST_OTHER_CLIENT_FLAGS								= ED_FLAG_OTHER_CLIENTS | ED_FLAG_ALL_CLIENTS,
	// All need to be broadcast to their client's logo
	ENTITY_BROADCAST_OWN_CLIENT_FLAGS								= ED_FLAG_ALL_CLIENTS | ED_FLAG_CELL_PUBLIC_AND_OWN | ED_FLAG_OWN_CLIENT | ED_FLAG_BASE_AND_CLIENT,
};

/** Mailbox corresponding to the category mapping component categories, the index on a table of a strict matching ENTITY MAILBOX TYPE values */
const COMPONENT_TYPE ENTITY_MAILBOX_COMPONENT_TYPE_MAPPING[] = 
{
	CELLAPP_TYPE,
	BASEAPP_TYPE,
	CLIENT_TYPE,
	BASEAPP_TYPE,
	CELLAPP_TYPE,
	CELLAPP_TYPE,
	BASEAPP_TYPE,
};

/** Property of Lod broadcast-level scope definition */
typedef uint8 DETAIL_TYPE;
#define DETAIL_LEVEL_NEAR													0	// LOD level：Near						
#define DETAIL_LEVEL_MEDIUM													1	// LOD level：medium
#define DETAIL_LEVEL_FAR													2	// LOD level：Far	

typedef std::map<std::string, EntityDataFlags> ENTITYFLAGMAP;
extern ENTITYFLAGMAP g_entityFlagMapping;										// entity 的flag字符串映射表

// UID category properties and methods
typedef uint16 ENTITY_PROPERTY_UID;
typedef uint16 ENTITY_METHOD_UID;
typedef uint16 ENTITY_SCRIPT_UID;
typedef uint16 DATATYPE_UID;
typedef uint8  DATATYPE;
typedef uint8  ENTITY_DEF_ALIASID;

#define DATA_TYPE_UNKONWN		0
#define DATA_TYPE_FIXEDARRAY	1
#define DATA_TYPE_FIXEDDICT		2
#define DATA_TYPE_STRING		3
#define DATA_TYPE_DIGIT			4
#define DATA_TYPE_BLOB			5
#define DATA_TYPE_PYTHON		6
#define DATA_TYPE_VECTOR		7
#define DATA_TYPE_UNICODE		8
#define DATA_TYPE_MAILBOX		9
#define DATA_TYPE_PYDICT		10
#define DATA_TYPE_PYTUPLE		11
#define DATA_TYPE_PYLIST		12

// Some system-level soft attributes on the entity are numbered in order to network to identify
enum ENTITY_BASE_PROPERTY_UTYPE
{
	ENTITY_BASE_PROPERTY_UTYPE_POSITION_XYZ					= 1,
	ENTITY_BASE_PROPERTY_UTYPE_DIRECTION_ROLL_PITCH_YAW		= 2,
	ENTITY_BASE_PROPERTY_UTYPE_SPACEID						= 3,
};

// Some system-level soft attributes on the entity are numbered in order to network to identify
enum ENTITY_BASE_PROPERTY_ALIASID
{
	ENTITY_BASE_PROPERTY_ALIASID_POSITION_XYZ				= 0,
	ENTITY_BASE_PROPERTY_ALIASID_DIRECTION_ROLL_PITCH_YAW	= 1,
	ENTITY_BASE_PROPERTY_ALIASID_SPACEID					= 2,
	ENTITY_BASE_PROPERTY_ALIASID_MAX						= 3,
};

// System properties is limited, def does not allow you to define
const char ENTITY_LIMITED_PROPERTYS[][32] =
{
	"id",
	"position",
	"direction",
	"spaceID",
	"autoLoad",
	"cell",
	"base",
	"client",
	"cellData",
	"className",
	"databaseID",
	"isDestroyed",
	"shouldAutoArchive",
	"shouldAutoBackup",
	"__ACCOUNT_NAME__",
	"__ACCOUNT_PASSWORD__",
	"clientAddr",
	"entitiesEnabled",
	"hasClient",
	"roundTripTime",
	"timeSinceHeardFromClient",
	"allClients",
	"hasWitness",
	"isWitnessed",
	"otherClients",
	"topSpeed",
	"topSpeedY",
	"",
};

}
#endif // KBENGINE_DEF_COMMON_H

