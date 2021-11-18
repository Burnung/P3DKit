project(testVray)

file(GLOB SOURCEFILES "*.h" "*.cpp")

#set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} ${SOURCEFILES})

set(IN_DIRS $ENV{VRAY_SDK}/cpp/include ${PROJECT_SOURCE_DIR} .. ../.. ${ASSIMP_INCLUDE} ${FREEIMG_INCLUDE}) 

include_directories(${IN_DIRS})


target_link_libraries(${PROJECT_NAME} common vrayRender)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER testCode)

find_package(Vulkan)
IF (Vulkan_FOUND)
    message(" Vulkan library!")
	include_directories(${Vulkan_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
	
ELSE()
	message("Cannot Find Vulkan")
ENDIF()