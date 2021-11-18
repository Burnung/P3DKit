project(loader)

file(GLOB SOURCEFILES "*.h" "*.cpp")

find_package(OpenCV 3.1 REQUIRED)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

#find_package(assimp REQUIRED)

add_library(${PROJECT_NAME} STATIC ${SOURCEFILES})

set(IN_DIRS ${PROJECT_SOURCE_DIR} .. ${ROOT_DIR} ${OPENCV_INCLUDE_DIRS} ${FREEIMG_INCLUDE})
IF (IS_WINDOWS)
    set(IN_DIRS ${IN_DIRS} ${ASSIMP_INCLUDE})
	message(${IN_DIRS})
	#file(GLOB assimp $ENV{ASSIMP_LIBRARY_PATH})
	message(assimp is ${assimp})
ENDIF()
include_directories(${IN_DIRS})

target_link_libraries(${PROJECT_NAME} common  geo  fbx ${assimp} ${freeimage})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER products)
