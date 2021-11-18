PROJECT(baseApp)
FILE(GLOB ALL_SRC_FILE "*h" "*.cpp" "*.hpp")

set(IN_DIRS ${ROOT_DIR} ${PROJECT_SOURCE_DIR} ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ..)
message(${IN_DIRS})
include_directories(${IN_DIRS})

add_library(${PROJECT_NAME} STATIC ${ALL_SRC_FILE})
getGL_libs(gl_libs)
target_link_libraries(${PROJECT_NAME} common geo P3DEngine ${gl_libs} )

find_package(Vulkan)
IF (Vulkan_FOUND AND WIN32)
    message(" Vulkan library!")
	message(${Vulkan_INCLUDE_DIRS})
	include_directories(${Vulkan_INCLUDE_DIRS})
	add_definitions(-DVULKAN_ENABLE=0)
	target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
ELSE()
	message("Cannot Find Vulkan")
ENDIF()

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER products)

