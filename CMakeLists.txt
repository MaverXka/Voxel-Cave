cmake_minimum_required(VERSION 4.0.2)
project(VoxelCave)

set (CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE cpp_sources CONFIGURE_DEPENDS
    src/*.cpp
    src/*.cxx
    src/*.cc
    src/*.h
    src/*.hpp
)

file(GLOB_RECURSE module_sources CONFIGURE_DEPENDS
    src/*.ixx
    src/*.cppm
)

function(group_sources_by_folder)
  foreach(file IN LISTS ARGN)

    file(RELATIVE_PATH relpath ${CMAKE_SOURCE_DIR} ${file})
    get_filename_component(dir ${relpath} PATH)
    if(dir STREQUAL "")
      set(dir ".")
    endif()

    string(REPLACE "/" "\\" group ${dir})
    source_group("${group}" FILES ${file})
  endforeach()
endfunction()

group_sources_by_folder(${cpp_sources} ${module_sources})

add_executable(VoxelCave WIN32)

target_compile_features(VoxelCave PUBLIC cxx_std_20)
target_include_directories(VoxelCave PUBLIC src/)
target_include_directories(VoxelCave PUBLIC include/)

target_sources(VoxelCave PRIVATE ${cpp_sources})

target_sources(VoxelCave PRIVATE FILE_SET CXX_MODULES
                                 BASE_DIRS src
                                 FILES ${module_sources}
                                )

target_compile_definitions(VoxelCave PRIVATE $<$<CONFIG:Debug>:DEBUG=1>)

set_target_properties(VoxelCave PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Content/"
)