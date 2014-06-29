cmake_minimum_required(VERSION 2.8)
project(mbl)

if( UNIX )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
set( MBL_SRC_PLATFORM
    src/unix/shared_mem.cpp
)
elseif( MSVC )
set( MBL_SRC_PLATFORM
    src/win32/shared_mem.cpp
)
endif( UNIX )

set( MBL_SRC_INCLUDES
    inc/mbl/shared_mem.h
    inc/mbl/lockless_state.h
    inc/mbl/flags.h
)

set( MBL_SRC_TEST
    tests/mbl_test.h
    tests/mbl_test.cpp
    tests/shared_mem_test.cpp
    tests/lockless_state_test.cpp
)

set( MBL_SRC_SHARED )
include_directories( inc )

set( MBL_SRC ${MBL_SRC_SHARED} ${MBL_SRC_PLATFORM} )
add_library(mbl STATIC ${MBL_SRC})
add_executable(mbl_test ${MBL_SRC_TEST})

target_link_libraries(mbl_test mbl)