project(testCuda LANGUAGES CUDA CXX)
enable_language(CUDA)

file(GLOB SOURCEFILES "*.h" "*.cpp" "*.cu" "*.cuh")
include(CheckLanguage)
check_language(CUDA)
find_package(OpenCV REQUIRED)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} ${SOURCEFILES})

set(IN_DIRS $ENV{VRAY_SDK}/cpp/include ${PROJECT_SOURCE_DIR} ../.. ${OPENCV_INCLUDE_DIRS})

include_directories(${IN_DIRS})
include_directories()

getGL_libs(gl_libs)
target_link_libraries(${PROJECT_NAME} common vrayRender geo P3DEngine ${OpenCV_LIBS} ${gl_libs})
target_link_libraries(${PROJECT_NAME} eigen lua zipper common)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER testCode)

##for CUDA
set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_SEPERABLE_COMPILATION ON)
target_link_libraries(${PROJECT_NAME} cudart_static cudnn)