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

#ifndef KBE_THREADTASK_H
#define KBE_THREADTASK_H

// common include	
// #define NDEBUG
#include "common/common.h"
#include "common/task.h"
#include "helper/debug_helper.h"

namespace KBEngine{ namespace thread{

/*
	Thread pool thread Cheng Ji class
*/

class TPTask : public Task
{
public:
	enum TPTaskState
	{
		/// A task has been completed
		TPTASK_STATE_COMPLETED = 0,

		/// Continue in the main thread execution
		TPTASK_STATE_CONTINUE_MAINTHREAD = 1,

		// Continue to thread
		TPTASK_STATE_CONTINUE_CHILDTHREAD = 2,
	};

	/**
		The return value： thread::TPTask::TPTaskState， Please refer to the TPTask State
	*/
	virtual thread::TPTask::TPTaskState presentMainThread(){ 
		return thread::TPTask::TPTASK_STATE_COMPLETED; 
	}
};

}
}

#endif // KBE_THREADTASK_H
