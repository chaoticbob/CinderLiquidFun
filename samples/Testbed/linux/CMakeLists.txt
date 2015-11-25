cmake_minimum_required( VERSION 3.1 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE on )
if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Debug" CACHE FILEPATH "" FORCE )
endif()

execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE CINDER_ARCH )
execute_process( COMMAND which clang OUTPUT_VARIABLE CLANG_PATH )

if( NOT "${CLANG_PATH}" STREQUAL "" )
    set( CINDER_TOOLCHAIN_CLANG true )
endif()

if( CINDER_TOOLCHAIN_CLANG )
    set( CMAKE_C_COMPILER						"clang" 		CACHE FILEPATH "" FORCE )
    set( CMAKE_CXX_COMPILER						"clang++" 		CACHE FILEPATH "" FORCE )
    set( CMAKE_AR          						"llvm-ar"		CACHE FILEPATH "" FORCE )
    set( CMAKE_LINKER       					"llvm-link" 	CACHE FILEPATH "" FORCE )
    set( CMAKE_NM           					"llvm-nm " 		CACHE FILEPATH "" FORCE )
    set( CMAKE_RANLIB       					"llvm-ranlib"	CACHE FILEPATH "" FORCE )

    set( CMAKE_C_FLAGS_INIT						"-Wall -std=c99" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_DEBUG_INIT				"-g" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_MINSIZEREL_INIT			"-Os -DNDEBUG" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_RELEASE_INIT				"-O4 -DNDEBUG" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS_RELWITHDEBINFO_INIT		"-O2 -g" CACHE STRING "" FORCE )
    set( CMAKE_C_FLAGS							"${CMAKE_C_FLAGS} -fmessage-length=0 " CACHE STRING "" FORCE )

    set( CMAKE_CXX_FLAGS_INIT					"-Wall" CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_DEBUG_INIT				"-g" CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_MINSIZEREL_INIT		"-Os -DNDEBUG" CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_RELEASE_INIT			"-O4 -DNDEBUG" CACHE STRING "" FORCE )
    set( CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT	"-O2 -g" CACHE STRING "" FORCE )
    set( CMAKE_CCC_FLAGS						"${CMAKE_C_FLAGS} -fmessage-length=0 " CACHE STRING "" FORCE )

    set( STDCXXLIB                           	"-stdlib=libstdc++" )
else()
    set( CMAKE_C_COMPILER	"gcc" 				CACHE FILEPATH "" FORCE )
    set( CMAKE_CXX_COMPILER	"g++" 				CACHE FILEPATH "" FORCE )
endif()

project( Testbed )

get_filename_component( SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src" ABSOLUTE )
get_filename_component( INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src" ABSOLUTE )

get_filename_component( BOX2D_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
get_filename_component( BOX2D_INC_DIR  "${BOX2D_DIR}/include" ABSOLUTE )
get_filename_component( BOX2D_LIB_DIR  "${BOX2D_DIR}/lib" ABSOLUTE )
get_filename_component( CINDER_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/../../../../.." ABSOLUTE )
get_filename_component( CINDER_INC_DIR "${CINDER_DIR}/include" ABSOLUTE )
get_filename_component( CINDER_LIB_DIR "${CINDER_DIR}/lib/linux/x86_64" ABSOLUTE )

include( ${CINDER_DIR}/linux/cmake/Cinder.cmake )

if( NOT TARGET cinder${CINDER_LIB_SUFFIX} )
    find_package( cinder REQUIRED
        PATHS ${CINDER_DIR}/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX}
    )
endif()

set( CXX_FLAGS "-std=c++11" )
if( CINDER_TOOLCHAIN_CLANG )
    set( CXX_FLAGS "${CXX_FLAGS} ${STDCXXLIB}" )
endif()
set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_FLAGS}" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLAGS}" )

if( UNIX )
    set( PLATFORM_DIR "linux/${CINDER_ARCH}" )
endif()

add_executable(
	${PROJECT_NAME}
	${SRC_DIR}/Tests/TestEntries.cpp
	${SRC_DIR}/Framework/Arrow.cpp
	${SRC_DIR}/Framework/FullscreenUI.cpp
	${SRC_DIR}/Framework/Main.cpp
	${SRC_DIR}/Framework/ParticleParameter.cpp
	${SRC_DIR}/Framework/Render.cpp
	${SRC_DIR}/Framework/Test.cpp	
)

if( CMAKE_BUILD_TYPE MATCHES "Debug" )
	set( LIB_SUFFIX "_d" )
endif()

target_include_directories( ${PROJECT_NAME} PUBLIC ${INC_DIR} ${BOX2D_INC_DIR} ${CINDER_INC_DIR} )
target_link_libraries( ${PROJECT_NAME} ${BOX2D_LIB_DIR}/${PLATFORM_DIR}/libliquidfun${LIB_SUFFIX}.a )
target_link_libraries( ${PROJECT_NAME} cinder${CINDER_LIB_SUFFIX} )
