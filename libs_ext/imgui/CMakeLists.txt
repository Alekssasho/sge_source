cmake_minimum_required(VERSION 2.6)
project(imgui)

add_dir_rec_2(SOURCES_IMGUI "./" 3)
add_library(imgui STATIC ${SOURCES_IMGUI})
target_include_directories(imgui PUBLIC "./")
#target_include_directories(imgui PUBLIC "../stb/")

