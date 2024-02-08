cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(raudio)

add_definitions(-DRAUDIO_STANDALONE)

add_library(raudio raudio.c external/stb_vorbis.c)
target_include_directories(raudio PUBLIC /external ./)
