#####################################################
# Project SGE Utils

cmake_minimum_required(VERSION 3.5)

project(sge_utils)

add_dir_rec_2(SOURCES_SGE_UTILS "./src" 3)
add_library(sge_utils STATIC ${SOURCES_SGE_UTILS})

target_include_directories(sge_utils PUBLIC "./src")

sgePromoteWarningsOnTarget(sge_utils)

#####################################################
# Project SGE Utils Tests
add_dir_rec_2(SOURCES_SGE_UTILS_TESTS "./tests" 3)
add_executable(sge_utils_Tests ${SOURCES_SGE_UTILS_TESTS})
target_link_libraries(sge_utils_Tests sge_utils)

target_include_directories(sge_utils_Tests PRIVATE "./tests")
target_include_directories(sge_utils_Tests PRIVATE "../../libs_ext/doctest/doctest")

sgePromoteWarningsOnTarget(sge_utils_Tests)