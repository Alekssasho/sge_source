#ifndef SGE_MCPP_H
#define SGE_MCPP_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*SgeFileLoaderFn)(const char* filename, const char** text, size_t* textNumCharacters, void* userData);

/// @param [out] output is the string containing the preprocessed file
/// @param [in/out] error is is a string containing all parse errors.
/// @param macroDefinitions is a list of additional macros to be defined (only when parsing the code, will not be defined in the output)
///        The list sould be in format "-DMACRO0 -DMACRO1"
/// @param [in] fileIdentifier the 1st file identifier to be passed to @getFileContents to kick-start the parsing process.
/// @param [in] a function used to obtain the files to be parsed.
/// @return non-zero if the prasing was successful
extern int sge_mcpp_preprocess(
    char** output, char** errors, const char* const* argv, int argc, SgeFileLoaderFn fileLoaderFn, void* fileLoaderFnUserData);


#ifdef __cplusplus
}
#endif

#endif
