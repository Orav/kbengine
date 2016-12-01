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

#ifndef KBE_STDFINDIF_HANDERS_H
#define KBE_STDFINDIF_HANDERS_H

#include "common/platform.h"

namespace KBEngine{

// Vector <string> like easy to use STD::find if to find the existence of a string
template<typename T>
class find_vec_string_exist_handle
{
public:
	find_vec_string_exist_handle(const std::basic_string< T >& str)
	: str_(str) {}

	bool operator()(const std::basic_string< T > &strSrc)
	{
		return strSrc == str_;
	}

	bool operator()(const T* strSrc)
	{
		return strSrc == str_;
	}

private:
	std::basic_string< T > str_;
};


// Vector <obj*> is easy to use, such as std::find if the presence of an object
template<typename T>
class findif_vector_obj_exist_handler
{
public:
	findif_vector_obj_exist_handler(T obj)
	: obj_(obj) {}

	bool operator()(const T &obj)
	{
		return obj == obj_;
	}

	bool operator()(const T* obj)
	{
		return obj == obj_;
	}

private:
	T obj_;
};

}

#endif // KBE_STDFINDIF_HANDERS_H
