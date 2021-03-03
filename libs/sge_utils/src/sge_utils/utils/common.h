#pragma once

#include <cstdio>

//----------------------------------------------------------------------------
// FILE operations.
//----------------------------------------------------------------------------
int sge_fopen(FILE** ppFile, const char* filename, const char* mode);
void sge_fclose_safe(FILE* pFile);


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
template <typename TStdVector>
void push_front(TStdVector& vec, const typename TStdVector::value_type& v) {
	if (vec.size() == 0)
		vec.push_back(v);
	else
		vec.insert(vec.begin(), v);
}
