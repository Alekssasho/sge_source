#####################################################
# mcpp
#####################################################
project(mcpp)

set(MCPP_SOURCES 
	./mcpp-2.7.2/src/system.c
	./mcpp-2.7.2/src/eval.c
	./mcpp-2.7.2/src/support.c
	./mcpp-2.7.2/src/directive.c
	./mcpp-2.7.2/src/expand.c
	./mcpp-2.7.2/src/mbchar.c
	./mcpp-2.7.2/src/preproc.c
	./mcpp-2.7.2/src/main.c
	./mcpp-2.7.2/src/sge_mcpp_file.c
	./mcpp-2.7.2/src/sge_mcpp_file.h
	./mcpp-2.7.2/src/sge_mcpp.h
	./mcpp-2.7.2/src/internal.H
	./mcpp-2.7.2/src/noconfig.H
)

add_library(mcpp STATIC ${MCPP_SOURCES})
target_compile_definitions(mcpp PRIVATE -DMCPP_LIB)
target_include_directories(mcpp PUBLIC ./mcpp-2.7.2/src)
