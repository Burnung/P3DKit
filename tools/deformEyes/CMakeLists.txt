project(deformTools)

file(GLOB SOURCEFILES "*.h" "*.cpp")

#set(CMAKE_INCLUDE_CURRENT_DIR ON)

#find_package(assimp REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

find_package(embree REQUIRED)
#set(SOURCEFILES main.cpp cartoon.h cartoon.cpp)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${EMBREE_INCLUDE_DIRS} ..)

add_library(${PROJECT_NAME} STATIC ${SOURCEFILES})

set(IN_DIRS ${PROJECT_SOURCE_DIR} .. ${ROOT_DIR})
IF (IS_WINDOWS)
    set(IN_DIRS ${IN_DIRS} ${ASSIMP_INCLUDE})
	message(${IN_DIRS})
	#file(GLOB assimp $ENV{ASSIMP_LIBRARY_PATH})
	message(assimp is ${assimp})
ENDIF()
include_directories(${IN_DIRS})

target_link_libraries(${PROJECT_NAME} common  eigen laplacian embreeScene)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER tools)
