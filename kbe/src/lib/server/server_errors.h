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

#ifndef KBE_SERVER_ERRORS_H
#define KBE_SERVER_ERRORS_H

#include "common/common.h"

namespace KBEngine { 

/**
	Server errors, mainly is used by the server back to the client.
*/
	
typedef uint16 SERVER_ERROR_CODE;										// Error code categories


#define SERVER_SUCCESS										0			// Success.
#define SERVER_ERR_SRV_NO_READY								1			// Server is not ready.
#define SERVER_ERR_SRV_OVERLOAD								2			// A server is heavily loaded.
#define SERVER_ERR_ILLEGAL_LOGIN							3			// Unauthorised access.
#define SERVER_ERR_NAME_PASSWORD							4			// User name or password is not correct.
#define SERVER_ERR_NAME										5			// User name is not correct.
#define SERVER_ERR_PASSWORD									6			// Password is not correct.
#define SERVER_ERR_ACCOUNT_CREATE_FAILED					7			// Failed to create account (one and the same account already exists).
#define SERVER_ERR_BUSY										8			// Operation is too busy (for example, in the case of a request did not complete before the server n times in a row create account).
#define SERVER_ERR_ACCOUNT_LOGIN_ANOTHER					9			// In another current account to log on.
#define SERVER_ERR_ACCOUNT_IS_ONLINE						10			// You've logged in, the server refused to log in again.
#define SERVER_ERR_PROXY_DESTROYED							11			// Associated with the client's proxy on the server have been destroyed.
#define SERVER_ERR_ENTITYDEFS_NOT_MATCH						12			// Does not match the entity defs.
#define SERVER_ERR_IN_SHUTTINGDOWN							13			// Server shutting down
#define SERVER_ERR_NAME_MAIL								14			// Wrong email address.
#define SERVER_ERR_ACCOUNT_LOCK								15			// Account is frozen.
#define SERVER_ERR_ACCOUNT_DEADLINE							16			// Account has expired.
#define SERVER_ERR_ACCOUNT_NOT_ACTIVATED					17			// Account is not activated.
#define SERVER_ERR_VERSION_NOT_MATCH						18			// And server versions do not match.
#define SERVER_ERR_OP_FAILED								19			// Operation failed.
#define SERVER_ERR_SRV_STARTING								20			// Server is started.
#define SERVER_ERR_ACCOUNT_REGISTER_NOT_AVAILABLE			21			// Account features are not yet open.
#define SERVER_ERR_CANNOT_USE_MAIL							22			// Email address cannot be used.
#define SERVER_ERR_NOT_FOUND_ACCOUNT						23			// This account was not found.
#define SERVER_ERR_DB										24			// Database error (check the dbMgr logs and DB).
#define SERVER_ERR_USER1									25			// User-defined error code 1
#define SERVER_ERR_USER2									26			// User-defined error code 2
#define SERVER_ERR_USER3									27			// User-defined error code 3
#define SERVER_ERR_USER4									28			// User-defined error code 4
#define SERVER_ERR_USER5									29			// User-defined error code 5
#define SERVER_ERR_USER6									30			// User-defined error code 6
#define SERVER_ERR_USER7									31			// User-defined error code 7
#define SERVER_ERR_USER8									32			// User-defined error code 8
#define SERVER_ERR_USER9									33			// User-defined error code 9
#define SERVER_ERR_USER10									34			// User-defined error code 10
#define SERVER_ERR_LOCAL_PROCESSING							35			// Local processing, something usually not processed by a third party, but by KBE server processing
#define SERVER_ERR_ACCOUNT_RESET_PASSWORD_NOT_AVAILABLE		36			// Open account password reset functionality.
#define SERVER_ERR_MAX										37          // Please put this on all the wrong end, this is not in itself an error identified, only that the total number of error definition

const char SERVER_ERR_STR[][256] = {
	"SERVER_SUCCESS",
	"SERVER_ERR_SRV_NO_READY",
	"SERVER_ERR_SRV_OVERLOAD",
	"SERVER_ERR_ILLEGAL_LOGIN",
	"SERVER_ERR_NAME_PASSWORD",
	"SERVER_ERR_NAME",
	"SERVER_ERR_PASSWORD",
	"SERVER_ERR_ACCOUNT_CREATE_FAILED",
	"SERVER_ERR_BUSY",
	"SERVER_ERR_ACCOUNT_LOGIN_ANOTHER",
	"SERVER_ERR_ACCOUNT_IS_ONLINE",
	"SERVER_ERR_PROXY_DESTROYED",
	"SERVER_ERR_ENTITYDEFS_NOT_MATCH",
	"SERVER_ERR_IN_SHUTTINGDOWN",
	"SERVER_ERR_NAME_MAIL",
	"SERVER_ERR_ACCOUNT_LOCK",
	"SERVER_ERR_ACCOUNT_DEADLINE",
	"SERVER_ERR_ACCOUNT_NOT_ACTIVATED",
	"SERVER_ERR_VERSION_NOT_MATCH",
	"SERVER_ERR_OP_FAILED",
	"SERVER_ERR_SRV_STARTING",
	"SERVER_ERR_ACCOUNT_REGISTER_NOT_AVAILABLE",
	"SERVER_ERR_CANNOT_USE_MAIL",
	"SERVER_ERR_NOT_FOUND_ACCOUNT",
	"SERVER_ERR_DB",
	"SERVER_ERR_USER1",
	"SERVER_ERR_USER2",
	"SERVER_ERR_USER3",
	"SERVER_ERR_USER4",
	"SERVER_ERR_USER5",
	"SERVER_ERR_USER6",
	"SERVER_ERR_USER7",
	"SERVER_ERR_USER8",
	"SERVER_ERR_USER9",
	"SERVER_ERR_USER10",
	"SERVER_ERR_LOCAL_PROCESSING",
	"SERVER_ERR_ACCOUNT_RESET_PASSWORD_NOT_AVAILABLE"
};

}

#endif // KBE_SERVER_ERRORS_H
