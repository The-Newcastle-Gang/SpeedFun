cmake_minimum_required(VERSION 3.0)
project(studio)

# configure projects
add_library(studio STATIC lib/x86/libfmodstudio_vc.lib) 

if (WIN32)
    target_link_libraries(studio PUBLIC winmm ws2_32)

    if(MSVC)
        target_compile_options(studio PRIVATE -W3)
    endif()
else()
    target_compile_options(studio PRIVATE -Wno-error)
endif()

target_include_directories(studio PUBLIC ${PROJECT_SOURCE_DIR}/include)

#if (ENET_SHARED)
#    add_library(enet_shared SHARED test/library.c)
#    target_compile_definitions(enet_shared PUBLIC -DENET_DLL)
#
#    if (WIN32)
#        target_link_libraries(enet_shared PUBLIC winmm ws2_32)
#
#        if(MSVC)
#            target_compile_options(enet_shared PRIVATE -W3)
#        endif()
#    else()
#        target_compile_options(enet_shared PRIVATE -Wno-error)
#    endif()
#
#    target_include_directories(enet_shared PUBLIC ${PROJECT_SOURCE_DIR}/include)
#endif()
#
#if (ENET_TEST)
#    add_executable(enet_test test/build.c)
#    target_include_directories(enet_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
#
#    if (WIN32)
#        target_link_libraries(enet_test PUBLIC winmm ws2_32)
#    endif()
#endif()

