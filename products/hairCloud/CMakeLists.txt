PROJECT(hairCloud)
FILE(GLOB Base_Files "*h" "*.cpp" "*.hpp")
FILE(GLOB_RECURSE Math_Files "./Math/*.h" "./Math/*.cpp" "./Math/*.hpp")
include_directories(. Math ${ROOT_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

set(All_Src_Files ${Base_Files} ${Math_Files})
add_library(${PROJECT_NAME} STATIC ${All_Src_Files})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER products)
target_link_libraries(${PROJECT_NAME} common)
