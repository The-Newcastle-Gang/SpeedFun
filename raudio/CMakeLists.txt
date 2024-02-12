CMAKE_MINIMUM_REQUIRED(VERSION 3.16.0 FATAL_ERROR)
project(raudio C)

add_library(raudio STATIC src/raudio.c)
target_include_directories(raudio PUBLIC /src /src/external)
target_compile_definitions(raudio PUBLIC -DRAUDIO_STANDALONE -DSUPPORT_FILEFORMAT_WAV -DSUPPORT_FILEFORMAT_OGG MMNOSOUND SUPPORT_MODULE_RAUDIO)


