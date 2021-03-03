#pragma once

#include <map>
#include <string>
#include <algorithm>

namespace sge
{

// A class assigning a unique id for every requested string.
// The zero is reserved and no string will be assigned with that variable.
struct StringRegister
{
	int getIndex(const std::string& str) 
	{
		unsigned res = strings[str];

		if(res!=0) return res;

		strings[str] = (int)strings.size();

		return (int)strings.size();
	}

private :
	std::map<std::string, int> strings;
};


}
