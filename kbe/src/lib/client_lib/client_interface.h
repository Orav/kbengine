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

#if defined(DEFINE_IN_INTERFACE)
	#undef KBE_CLIENT_INTERFACE_H
#endif


#ifndef KBE_CLIENT_INTERFACE_H
#define KBE_CLIENT_INTERFACE_H

// common include	
#if defined(CLIENT)
#include "clientapp.h"
#endif
#include "client_interface_macros.h"
#include "network/interface_defs.h"
#include "server/server_errors.h"
#include "entitydef/common.h"
#include "common.h"
	
namespace KBEngine{

/**
	All CLIENT messaging interface defined here
*/
NETWORK_INTERFACE_DECLARE_BEGIN(ClientInterface)
	// Server Hello returned.
	CLIENT_MESSAGE_DECLARE_STREAM(onHelloCB,								NETWORK_VARIABLE_MESSAGE)

	// And server versions do not match
	CLIENT_MESSAGE_DECLARE_STREAM(onVersionNotMatch,						NETWORK_VARIABLE_MESSAGE)

	// And the server does not match the version of the script
	CLIENT_MESSAGE_DECLARE_STREAM(onScriptVersionNotMatch,					NETWORK_VARIABLE_MESSAGE)

	// Failed to create account.
	CLIENT_MESSAGE_DECLARE_STREAM(onCreateAccountResult,					NETWORK_VARIABLE_MESSAGE)

	// A successful logon.
	CLIENT_MESSAGE_DECLARE_STREAM(onLoginSuccessfully,						NETWORK_VARIABLE_MESSAGE)

	// Login failed.
	CLIENT_MESSAGE_DECLARE_STREAM(onLoginFailed,							NETWORK_VARIABLE_MESSAGE)

	// Has created a server-side proxy associated with the client Entity | | Login gateway is successful.
	CLIENT_MESSAGE_DECLARE_ARGS3(onCreatedProxies,							NETWORK_VARIABLE_MESSAGE,
								uint64,										rndUUID,
								ENTITY_ID,									eid,
								std::string,								entityType)

	// Login gateway fails.
	CLIENT_MESSAGE_DECLARE_ARGS1(onLoginBaseappFailed,						NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Login gateway fails.
	CLIENT_MESSAGE_DECLARE_ARGS1(onReLoginBaseappFailed,					NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Entity has entered the game on the server world.
	CLIENT_MESSAGE_DECLARE_STREAM(onEntityEnterWorld,						NETWORK_VARIABLE_MESSAGE)

	// Entity has to leave the game on the server world.
	CLIENT_MESSAGE_DECLARE_ARGS1(onEntityLeaveWorld,						NETWORK_FIXED_MESSAGE,
								ENTITY_ID,									eid)

	// Entity has to leave the game on the server world.
	CLIENT_MESSAGE_DECLARE_STREAM(onEntityLeaveWorldOptimized,				NETWORK_VARIABLE_MESSAGE)

	// Tell the client whether a entity destroyed, this type of entity is usually not on entity enter world.
	CLIENT_MESSAGE_DECLARE_ARGS1(onEntityDestroyed,							NETWORK_FIXED_MESSAGE,
								ENTITY_ID,									eid)

	// Entity has entered the space on the server.
	CLIENT_MESSAGE_DECLARE_STREAM(onEntityEnterSpace,						NETWORK_VARIABLE_MESSAGE)

	// Entity had left space on the server.
	CLIENT_MESSAGE_DECLARE_ARGS1(onEntityLeaveSpace,						NETWORK_FIXED_MESSAGE,
								ENTITY_ID,									eid)

	// Remote call entity methods
	CLIENT_MESSAGE_DECLARE_STREAM(onRemoteMethodCall,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onRemoteMethodCallOptimized,				NETWORK_VARIABLE_MESSAGE)

	// Kicked out of the server
	CLIENT_MESSAGE_DECLARE_ARGS1(onKicked,									NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Server Update entity properties
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdatePropertys,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdatePropertysOptimized,				NETWORK_VARIABLE_MESSAGE)

	// Server sets the position of the entity and
	CLIENT_MESSAGE_DECLARE_STREAM(onSetEntityPosAndDir,						NETWORK_VARIABLE_MESSAGE)

	// Server update package
	CLIENT_MESSAGE_DECLARE_ARGS3(onUpdateBasePos,							NETWORK_FIXED_MESSAGE,
								float,										x,
								float,										y,
								float,										z)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateBaseDir,							NETWORK_VARIABLE_MESSAGE)

	CLIENT_MESSAGE_DECLARE_ARGS2(onUpdateBasePosXZ,							NETWORK_FIXED_MESSAGE,
								float,										x,
								float,										z)

	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData,								NETWORK_VARIABLE_MESSAGE)

	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_ypr,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_yp,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_yr,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_pr,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_y,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_p,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_r,							NETWORK_VARIABLE_MESSAGE)

	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_ypr,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_yp,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_yr,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_pr,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_y,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_p,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xz_r,						NETWORK_VARIABLE_MESSAGE)

	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz,							NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_ypr,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_yp,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_yr,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_pr,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_y,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_p,						NETWORK_VARIABLE_MESSAGE)
	CLIENT_MESSAGE_DECLARE_STREAM(onUpdateData_xyz_r,						NETWORK_VARIABLE_MESSAGE)

	// Download stream begins 
	CLIENT_MESSAGE_DECLARE_ARGS3(onStreamDataStarted,						NETWORK_VARIABLE_MESSAGE,
								int16,										id,
								uint32,										datasize,
								std::string,								descr)

	// Receiving stream data
	CLIENT_MESSAGE_DECLARE_STREAM(onStreamDataRecv,							NETWORK_VARIABLE_MESSAGE)

	// Download stream has completed 
	CLIENT_MESSAGE_DECLARE_ARGS1(onStreamDataCompleted,						NETWORK_FIXED_MESSAGE,
								int16,										id)

	// Import protocols
	CLIENT_MESSAGE_DECLARE_STREAM(onImportClientMessages,					NETWORK_VARIABLE_MESSAGE)
	
	// Import entitydef
	CLIENT_MESSAGE_DECLARE_STREAM(onImportClientEntityDef,					NETWORK_VARIABLE_MESSAGE)

	// Error code that describes the exported
	CLIENT_MESSAGE_DECLARE_STREAM(onImportServerErrorsDescr,				NETWORK_VARIABLE_MESSAGE)

	// Server-side initialization spacedata
	CLIENT_MESSAGE_DECLARE_STREAM(initSpaceData,							NETWORK_VARIABLE_MESSAGE)

	// Server settings spacedata
	CLIENT_MESSAGE_DECLARE_ARGS3(setSpaceData,								NETWORK_VARIABLE_MESSAGE,
								SPACE_ID,									spaceID,
								std::string,								key,
								std::string,								val)

	// Server delete spacedata
	CLIENT_MESSAGE_DECLARE_ARGS2(delSpaceData,								NETWORK_VARIABLE_MESSAGE,
								SPACE_ID,									spaceID,
								std::string,								key)

	// Reset account password request returns
	CLIENT_MESSAGE_DECLARE_ARGS1(onReqAccountResetPasswordCB,				NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Reset account password request returns
	CLIENT_MESSAGE_DECLARE_ARGS1(onReqAccountBindEmailCB,					NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// Reset account password request returns
	CLIENT_MESSAGE_DECLARE_ARGS1(onReqAccountNewPasswordCB,					NETWORK_FIXED_MESSAGE,
								SERVER_ERROR_CODE,							failedcode)

	// To visit the gateway successfully 
	CLIENT_MESSAGE_DECLARE_STREAM(onReLoginBaseappSuccessfully,				NETWORK_VARIABLE_MESSAGE)
									
	// Tell clients: you are responsible for (or cancel) whose displacement control synchronization
	CLIENT_MESSAGE_DECLARE_ARGS2(onControlEntity,							NETWORK_FIXED_MESSAGE,
									ENTITY_ID,								eid,
									int8,									isControlled)

	// Server heartbeat callbacks
	CLIENT_MESSAGE_DECLARE_ARGS0(onAppActiveTickCB,							NETWORK_FIXED_MESSAGE)

	NETWORK_INTERFACE_DECLARE_END()

#ifdef DEFINE_IN_INTERFACE
	#undef DEFINE_IN_INTERFACE
#endif

}

#endif // KBE_CLIENT_INTERFACE_H
