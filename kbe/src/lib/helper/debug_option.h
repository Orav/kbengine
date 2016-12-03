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


#ifndef KBE_DEBUG_OPTION_H
#define KBE_DEBUG_OPTION_H
#include "common/common.h"

namespace KBEngine{

namespace Network
{

/** 
	This switch sets the length of the packet is always to bring information that way in front of the coupling provides a few convenience
	 If false the length of fixed-length packets do not carry information up to the end to resolve
*/
extern bool g_packetAlwaysContainLength;

/**
Any incoming and outgoing packages need to be provided in the text output to the log debug
		g_trace_packet:
			0: Do not output
			1: 16 input and output
			2: Stream output
			3: 10 input and output
		use_logfile:
			Independent of a log file to record the package contents, the file name is usually
			appname_packetlogs.log
		g_trace_packet_disables:
			Close some package output
*/
extern uint8 g_trace_packet;
extern bool g_trace_encrypted_packet;
extern std::vector<std::string> g_trace_packet_disables;
extern bool g_trace_packet_use_logfile;

}

/**
	Whether to output entity created, the script gets the property and initialization properties for debugging information.
*/
extern bool g_debugEntity;

/**
	Apps published indefinitely, in script, you can get the value
		0 : debug
		1 : release
*/
extern int8 g_appPublish;

}

#endif // KBE_DEBUG_OPTION_H
