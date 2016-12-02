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

/*
	Thread guards:
	Avoid malicious competition between the threads deadlock.
		Usage:
		Each other members defined within a class
		Thread mutex tm;
		Areas in need of protection:
		void XXCLASS::foo(void)
			{
			Thread guard tg(this->tm);
			The following code was safe
		... }
*/
#ifndef KBE_THREADGUARD_H
#define KBE_THREADGUARD_H
	
#include "thread/threadmutex.h"
#include <assert.h>
	
namespace KBEngine{ namespace thread{

class ThreadGuard
{
public:
	explicit ThreadGuard(ThreadMutex* mutexPtr):mutexPtr_(mutexPtr)
	{
		mutexPtr_->lockMutex();
	}

	virtual ~ThreadGuard(void) 
	{ 
		mutexPtr_->unlockMutex();
	}	
	
protected:
	ThreadMutex* mutexPtr_;
};

}
}

#endif // KBE_THREADGUARD_H
