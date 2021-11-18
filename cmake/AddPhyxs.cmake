message(build type ${CMAKE_BUILD_TYPE})    

include_directories(${ROOT_DIR}/libs/physx/windows/PhysX/include ${ROOT_DIR}/libs/physx/windows/PxShared/include)

function(get_physx_libs libs_name)
    IF(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(lib_dir ${ROOT_DIR}/libs/physx/windows/PhysX/bin/release)
    ELSE()
        set(lib_dir ${ROOT_DIR}/libs/physx/windows/PhysX/bin/release)
    ENDIF()
    message(link library is ${lib_dir})
    link_directories(${lib_dir})
    file(GLOB all_libs ${lib_dir}/*.lib) 
FILE(GLOB Base_Files "*h" "*.cpp" "*.hpp")
        set(${libs_name} ${all_libs} PARENT_SCOPE)
endfunction()

# Call into PhysX's CMake scripts