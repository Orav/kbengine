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


#include "math.h"
namespace KBEngine{ namespace script{ namespace math {

//-------------------------------------------------------------------------------------
bool installModule(const char* moduleName)
{
	// Initializes a math-related modules
	PyObject *mathModule = PyImport_AddModule(moduleName);
	static struct PyModuleDef moduleDesc =   
	{  
			 PyModuleDef_HEAD_INIT,  
			 "Math",  
			 "This module is created by KBEngine!",  
			 -1,  
			 NULL  
	};  

	PyModule_Create(&moduleDesc);			
	
	// Initialize Script Vector2
	script::ScriptVector2::installScript(mathModule, "Vector2");
	// Initialize Script Vector3
	script::ScriptVector3::installScript(mathModule, "Vector3");
	// Initialize Script Vector4
	script::ScriptVector4::installScript(mathModule, "Vector4");
	return true;
}

//-------------------------------------------------------------------------------------
bool uninstallModule()
{
	script::ScriptVector2::uninstallScript();
	script::ScriptVector3::uninstallScript();
	script::ScriptVector4::uninstallScript();
	return true;
}

}
}
}
