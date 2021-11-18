project(testVulkan)
find_package(glfw3 REQUIRED)
find_package(glew REQUIRED STATIC)
file(GLOB SOURCEFILES "*.h" "*.cpp")

find_package(OpenCV REQUIRED)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} ${SOURCEFILES})
find_package(Vulkan)

set(IN_DIRS $ENV{VRAY_SDK}/cpp/include ${PROJECT_SOURCE_DIR} ../..${OPENCV_INCLUDE_DIRS})

include_directories(${IN_DIRS})

target_link_libraries(${PROJECT_NAME} common vrayRender common geo P3DEngine glfw  ${Vulkan_LIBRARIES} ${OpenCV_LIBS} ${Vulkan_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER testCode)