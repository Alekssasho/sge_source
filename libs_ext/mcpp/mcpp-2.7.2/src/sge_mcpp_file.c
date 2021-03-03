#include "sge_mcpp_file.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

SgeFileLoaderFn g_sgeFileLoaderFn = NULL;
void* g_sgeFileLoaderUserData = NULL;

SGE_MCPP_FILE* sge_mcpp_fopen(const char* filename, char* fopenModUnused) {
	char* text = NULL;
	size_t textNumCharacters = 0;
	if (g_sgeFileLoaderFn(filename, &text, &textNumCharacters, g_sgeFileLoaderUserData) != 0) {
		SGE_MCPP_FILE* sgeFile = (SGE_MCPP_FILE*)malloc(sizeof(SGE_MCPP_FILE));
		sgeFile->pBegin = text;
		sgeFile->pEnd = text + textNumCharacters;
		sgeFile->pIterator = sgeFile->pBegin;

		return sgeFile;
	}

	return NULL;
}

void sge_mcpp_fclose(SGE_MCPP_FILE* sgeFile) {
	if ((sgeFile != NULL) && (sgeFile != (SGE_MCPP_FILE*)(-1))) {
		free(sgeFile);
	}
}

void sge_mcpp_fseek(SGE_MCPP_FILE* sgeFile, int offset, int origin) {
	if (sgeFile->pBegin == NULL) {
		sgeFile->pIterator = NULL;
		return;
	}

	if(origin == SEEK_CUR) {
		sgeFile->pIterator = sgeFile->pIterator + offset;
	}

	if(origin == SEEK_END) {
		sgeFile->pIterator = sgeFile->pEnd - offset;
	}

	if(origin == SEEK_SET) {
		sgeFile->pIterator = sgeFile->pBegin + offset;
	}
	
	
	if (sgeFile->pIterator < sgeFile->pBegin) {
		sgeFile->pIterator = sgeFile->pBegin;
	}

	if (sgeFile->pIterator > sgeFile->pEnd) {
		sgeFile->pIterator = sgeFile->pEnd;
	}
}

extern long sge_mcpp_ftell(SGE_MCPP_FILE* sgeFile) {
	if (sgeFile) {
		return (int)(sgeFile->pIterator - sgeFile->pBegin);
	}
	return 0;
}

// https://www.cplusplus.com/reference/cstdio/fgets/
// Reads characters from stream and stores them as a C string into str until (num-1) characters
// have been read or either a newline or the end-of-file is reached, whichever happens first.
// A newline character makes fgets stop reading, but it is considered a valid character by the function
// and included in the string copied to str.
// A terminating null character is automatically appended after the characters copied to str.
// Notice that fgets is quite different from gets: not only fgets accepts a stream argument,
// but also allows to specify the maximum size of str and includes in the string any ending newline character.
char* sge_mcpp_fgets(char* str, int size, SGE_MCPP_FILE* sgeFile) {
	if (str == NULL) {
		return NULL;
	}

	if (sgeFile->pIterator >= sgeFile->pEnd) {
		return NULL;
	}

	int t = 0;
	while (t < (size-1)) {
		if (sgeFile->pIterator < sgeFile->pEnd) {
			char ch = *sgeFile->pIterator;
			sgeFile->pIterator++;
			str[t] = ch;

			if (ch == '\0' || ch == '\n') {
				++t;
				break;
			}
		} else {
			break;
		}

		++t;
	}

	str[t] = '\0';

	return str;
}

int sge_mcpp_ferror(SGE_MCPP_FILE* sgeFile) {
	return 0;
}
