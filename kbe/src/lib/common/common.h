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
#ifndef KBE_COMMON_H
#define KBE_COMMON_H
#include "common/platform.h"
#include "common/singleton.h"
#include "common/kbeversion.h"
#include "common/kbemalloc.h"
#include "common/stringconv.h"
#include "common/format.h"

namespace KBEngine{

/** Safe release a pointer memory */
#define SAFE_RELEASE(i)										\
	if (i)													\
		{													\
			delete i;										\
			i = NULL;										\
		}

/** Safe release of a pointer array memory */
#define SAFE_RELEASE_ARRAY(i)								\
	if (i)													\
		{													\
			delete[] i;										\
			i = NULL;										\
		}

#ifdef CODE_INLINE
    #define INLINE    inline
#else
    #define INLINE
#endif

/** kbe Time */
extern GAME_TIME g_kbetime;

/** Account categories */
enum ACCOUNT_TYPE
{
	ACCOUNT_TYPE_NORMAL = 1,	// Ordinary account
	ACCOUNT_TYPE_MAIL = 2,		// Email account (required to activate)
	ACCOUNT_TYPE_SMART = 3		// Intelligent recognition
};

enum ACCOUNT_FLAGS
{
	ACCOUNT_FLAG_NORMAL = 0x00000000,
	ACCOUNT_FLAG_LOCK = 0x000000001,
	ACCOUNT_FLAG_NOT_ACTIVATED = 0x000000002
};

/** Entity type of mailbox */
enum ENTITY_MAILBOX_TYPE
{
	MAILBOX_TYPE_CELL												= 0,
	MAILBOX_TYPE_BASE												= 1,
	MAILBOX_TYPE_CLIENT												= 2,
	MAILBOX_TYPE_CELL_VIA_BASE										= 3,
	MAILBOX_TYPE_BASE_VIA_CELL										= 4,
	MAILBOX_TYPE_CLIENT_VIA_CELL									= 5,
	MAILBOX_TYPE_CLIENT_VIA_BASE									= 6,
};

/** Mailbox class of transpositions and ENTITY names are stringent MAILBOX TYPE for string index matching */
const char ENTITY_MAILBOX_TYPE_TO_NAME_TABLE[][8] = 
{
	"cell",
	"base",
	"client",
	"cell",
	"base",
	"client",
	"client",
};

/** Mailbox class of transpositions and ENTITY names are stringent MAILBOX TYPE for string index matching */
const char ENTITY_MAILBOX_TYPE_TO_NAME_TABLE_EX[][14] =
{
	"cell",
	"base",
	"client",
	"cellViaBase",
	"baseViaCell",
	"clientViaCell",
	"clientViaBase",
};

/** Define server component status */
enum COMPONENT_STATE
{
	// Initial state
	COMPONENT_STATE_INIT = 0,

	// Process is running
	COMPONENT_STATE_RUN = 1,

	// Processes started to close
	COMPONENT_STATE_SHUTTINGDOWN_BEGIN = 2,

	// Process is shutting down
	COMPONENT_STATE_SHUTTINGDOWN_RUNNING = 3,

	// Process shutdown complete
	COMPONENT_STATE_STOP = 4
};

/** Define the server components category */
enum COMPONENT_TYPE
{
	UNKNOWN_COMPONENT_TYPE	= 0,
	DBMGR_TYPE				= 1,
	LOGINAPP_TYPE			= 2,
	BASEAPPMGR_TYPE			= 3,
	CELLAPPMGR_TYPE			= 4,
	CELLAPP_TYPE			= 5,
	BASEAPP_TYPE			= 6,
	CLIENT_TYPE				= 7,
	MACHINE_TYPE			= 8,
	CONSOLE_TYPE			= 9,
	LOGGER_TYPE				= 10,
	BOTS_TYPE				= 11,
	WATCHER_TYPE			= 12,
	INTERFACES_TYPE			= 13,
	COMPONENT_END_TYPE		= 14,
};

/** Current server components category andID */
extern COMPONENT_TYPE g_componentType;
extern COMPONENT_ID g_componentID;

/** Define the name server components */
const char COMPONENT_NAME[][255] = {
	"unknown",
	"dbmgr",
	"loginapp",
	"baseappmgr",
	"cellappmgr",
	"cellapp",
	"baseapp",
	"client",
	"machine",
	"console",
	"logger",
	"bots",
	"watcher",
	"interfaces",
};

const char COMPONENT_NAME_1[][255] = {
	"unknown   ",
	"dbmgr     ",
	"loginapp  ",
	"baseappmgr",
	"cellappmgr",
	"cellapp   ",
	"baseapp   ",
	"client    ",
	"machine   ",
	"console   ",
	"logger    ",
	"bots      ",
	"watcher   ",
	"interfaces",
};

const char COMPONENT_NAME_2[][255] = {
	"   unknown",
	"     dbmgr",
	"  loginapp",
	"baseappmgr",
	"cellappmgr",
	"   cellapp",
	"   baseapp",
	"    client",
	"   machine",
	"   console",
	"    logger",
	"      bots",
	"   watcher",
	"interfaces",
};

inline const char* COMPONENT_NAME_EX(COMPONENT_TYPE CTYPE)
{									
	if(CTYPE < 0 || CTYPE >= COMPONENT_END_TYPE)
	{
		return COMPONENT_NAME[UNKNOWN_COMPONENT_TYPE];
	}

	return COMPONENT_NAME[CTYPE];
}

inline const char* COMPONENT_NAME_EX_1(COMPONENT_TYPE CTYPE)
{									
	if(CTYPE < 0 || CTYPE >= COMPONENT_END_TYPE)
	{
		return COMPONENT_NAME_1[UNKNOWN_COMPONENT_TYPE];
	}

	return COMPONENT_NAME_1[CTYPE];
}

inline const char* COMPONENT_NAME_EX_2(COMPONENT_TYPE CTYPE)
{									
	if(CTYPE < 0 || CTYPE >= COMPONENT_END_TYPE)
	{
		return COMPONENT_NAME_2[UNKNOWN_COMPONENT_TYPE];
	}

	return COMPONENT_NAME_2[CTYPE];
}

inline COMPONENT_TYPE ComponentName2ComponentType(const char* name)
{
	for(int i=0; i<(int)COMPONENT_END_TYPE; ++i)
	{
		if(kbe_stricmp(COMPONENT_NAME[i], name) == 0)
			return (COMPONENT_TYPE)i;
	}

	return UNKNOWN_COMPONENT_TYPE;
}

// List of all components
const COMPONENT_TYPE ALL_COMPONENT_TYPES[] = {BASEAPPMGR_TYPE, CELLAPPMGR_TYPE, DBMGR_TYPE, CELLAPP_TYPE, 
						BASEAPP_TYPE, LOGINAPP_TYPE, MACHINE_TYPE, CONSOLE_TYPE, LOGGER_TYPE, 
						WATCHER_TYPE, INTERFACES_TYPE, BOTS_TYPE, UNKNOWN_COMPONENT_TYPE};

// List of all back-end components
const COMPONENT_TYPE ALL_SERVER_COMPONENT_TYPES[] = {BASEAPPMGR_TYPE, CELLAPPMGR_TYPE, DBMGR_TYPE, CELLAPP_TYPE, 
						BASEAPP_TYPE, LOGINAPP_TYPE, MACHINE_TYPE, LOGGER_TYPE, 
						WATCHER_TYPE, INTERFACES_TYPE, BOTS_TYPE, UNKNOWN_COMPONENT_TYPE};

// List of all back-end components
const COMPONENT_TYPE ALL_GAME_SERVER_COMPONENT_TYPES[] = {BASEAPPMGR_TYPE, CELLAPPMGR_TYPE, DBMGR_TYPE, CELLAPP_TYPE, 
						BASEAPP_TYPE, LOGINAPP_TYPE, INTERFACES_TYPE, UNKNOWN_COMPONENT_TYPE};

// All auxiliary components
const COMPONENT_TYPE ALL_HELPER_COMPONENT_TYPE[] = {LOGGER_TYPE, UNKNOWN_COMPONENT_TYPE};

// Returned is a valid component
#define VALID_COMPONENT(C_TYPE) ((C_TYPE) > 0 && (C_TYPE) < COMPONENT_END_TYPE)

/** Check for a game server components category */
inline bool isGameServerComponentType(COMPONENT_TYPE componentType)
{
	int i = 0;
	while(true)
	{
		COMPONENT_TYPE t = ALL_GAME_SERVER_COMPONENT_TYPES[i++];
		if(t == UNKNOWN_COMPONENT_TYPE)
			break;

		if(t == componentType)
			return true;
	}

	return false;
}

// Front-end application categories, All client type
enum COMPONENT_CLIENT_TYPE
{
	UNKNOWN_CLIENT_COMPONENT_TYPE	= 0,

	// Mobile, Phone, Pad
	CLIENT_TYPE_MOBILE				= 1,

	// Windows Application program
	CLIENT_TYPE_WIN					= 2,

	// 独立的Linux应用程序
	// Linux Application program
	CLIENT_TYPE_LINUX				= 3,
		
	// Mac Application program
	CLIENT_TYPE_MAC					= 4,
				
	// Web, HTML5, Flash
	CLIENT_TYPE_BROWSER				= 5,

	// bots
	CLIENT_TYPE_BOTS				= 6,

	// Light side classes
	CLIENT_TYPE_MINI				= 7,

	// End
	CLIENT_TYPE_END					= 8
};

/** Defining front-end application category names */
const char COMPONENT_CLIENT_NAME[][255] = {
	"UNKNOWN_CLIENT_COMPONENT_TYPE",
	"CLIENT_TYPE_MOBILE",
	"CLIENT_TYPE_WIN",
	"CLIENT_TYPE_LINUX",
	"CLIENT_TYPE_MAC",
	"CLIENT_TYPE_BROWSER",
	"CLIENT_TYPE_BOTS",
	"CLIENT_TYPE_MINI",
};

// All front-end application categories
const COMPONENT_CLIENT_TYPE ALL_CLIENT_TYPES[] = {CLIENT_TYPE_MOBILE, CLIENT_TYPE_WIN, CLIENT_TYPE_LINUX, CLIENT_TYPE_MAC, 
												CLIENT_TYPE_BROWSER, CLIENT_TYPE_BOTS, CLIENT_TYPE_MINI, UNKNOWN_CLIENT_COMPONENT_TYPE};

typedef int8 CLIENT_CTYPE;

/*
 Sign of APP settings
*/
// The default (no tags)
#define APP_FLAGS_NONE								0x00000000
// Do not participate in load balancing
#define APP_FLAGS_NOT_PARTCIPATING_LOAD_BALANCING	0x00000001

// Create a tag worthy of names to map and provide exposure to initialize Python for scripting use
inline std::map<uint32, std::string> createAppFlagsMaps()
{
	std::map<uint32, std::string> datas;
	datas[APP_FLAGS_NONE] = "APP_FLAGS_NONE";
	datas[APP_FLAGS_NOT_PARTCIPATING_LOAD_BALANCING] = "APP_FLAGS_NOT_PARTCIPATING_LOAD_BALANCING";
	return datas;
}

// Front end supports floating point numbers
// #define CLIENT_NO_FLOAT

// The default border of a cell or minimum size
#define CELL_DEF_MIN_AREA_SIZE						500.0f

/** A space a chunk size */
#define SPACE_CHUNK_SIZE							100


/** Check the user name validity */
inline bool validName(const char* name, int size)
{
	if(size >= 256)
		return false;

	for(int i=0; i<size; ++i)
	{
		char ch = name[i];
		if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || (ch == '_'))
			continue;

		return false;
	}

	return true;
}

inline bool validName(const std::string& name)
{
	return validName(name.c_str(), (int)name.size());
}

/** Check the email address validity 
Match exactly with the following expression
[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?
*/
#ifdef USE_REGEX
#include <regex>
#endif

inline bool email_isvalid(const char *address) 
{
#ifdef USE_REGEX
	std::tr1::regex _mail_pattern("([a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?)");
	return std::tr1::regex_match(accountName, _mail_pattern);
#endif
	int len = (int)strlen(address);
	if(len <= 3)
		return false;

	char ch = address[len - 1];
	if(!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')))
		return false;

	int        count = 0;
	const char *c, *domain;
	static const char *rfc822_specials = "()<>@,;:\\\"[]";

	/* first we validate the name portion (name@domain) */
	for (c = address;  *c;  c++) {
	if (*c == '\"' && (c == address || *(c - 1) == '.' || *(c - 1) == 
		'\"')) {
	  while (*++c) {
		if (*c == '\"') break;
		if (*c == '\\' && (*++c == ' ')) continue;
		if (*c <= ' ' || *c >= 127) return false;
	  }
	  if (!*c++) return false;
	  if (*c == '@') break;
	  if (*c != '.') return false;
	  continue;
	}
	if (*c == '@') break;
	if (*c <= ' ' || *c >= 127) return false;
	if (strchr(rfc822_specials, *c)) return false;
	}
	if (c == address || *(c - 1) == '.') return false;

	/* next we validate the domain portion (name@domain) */
	if (!*(domain = ++c)) return false;
	do {
	if (*c == '.') {
	  if (c == domain || *(c - 1) == '.') return false;
	  count++;
	}
	if (*c <= ' ' || *c >= 127) return false;
	if (strchr(rfc822_specials, *c)) return false;
	} while (*++c);

	return (count >= 1);
}

}
#endif // KBE_COMMON_H
