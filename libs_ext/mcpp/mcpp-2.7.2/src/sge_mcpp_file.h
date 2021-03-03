#ifndef SGE_MCPP_FILE_H
#define SGE_MCPP_FILE_H

typedef int (*SgeFileLoaderFn)(const char* filename, const char** text, size_t* textNumCharacters, void* userData);

typedef struct SGE_MCPP_FILE_TAG {
    char* pIterator;
    char* pBegin;
    char* pEnd;
} SGE_MCPP_FILE;

extern SGE_MCPP_FILE* sge_mcpp_fopen(const char* filename, char* fopenModUnused);
extern void sge_mcpp_fclose(SGE_MCPP_FILE* sgeFile);
extern void sge_mcpp_fseek(SGE_MCPP_FILE* sgeFile, int offset, int origin);
extern long sge_mcpp_ftell(SGE_MCPP_FILE* sgeFile);
extern char* sge_mcpp_fgets(char* str, int size, SGE_MCPP_FILE* sgeFile);

/// Global variables used to enable the user to "virtually" load SGE_MCPP_FILEs.
extern SgeFileLoaderFn g_sgeFileLoaderFn;
extern void* g_sgeFileLoaderUserData;

extern int sge_mcpp_ferror(SGE_MCPP_FILE* sgeFile);
extern int sge_mcpp_tell(SGE_MCPP_FILE* sgeFile);

#endif