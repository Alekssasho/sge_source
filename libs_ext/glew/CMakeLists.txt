#####################################################
# Project GLEW

cmake_minimum_required(VERSION 3.5)

project(glew)

add_library(glew STATIC ./glew.c)
target_include_directories(glew PUBLIC "./")