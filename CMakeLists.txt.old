cmake_minimum_required(VERSION 3.22.1)

#================================================
# Variables and Files
#================================================

set(APPLICATION_NAME "muser")
set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/res)
set(SOURCE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/headers)
set(OUT_DIR ${CMAKE_SOURCE_DIR}/build/bin)

file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.c" "${CMAKE_SOURCE_DIR}/src/*.cc")
file(GLOB_RECURSE SRC_HEADERS "${CMAKE_SOURCE_DIR}/src/headers/*.hpp" "${CMAKE_SOURCE_DIR}/src/headers/*.h")
file(GLOB_RECURSE RESOURCES "${RESOURCE_DIR}/*")

#================================================
# Project Settings
#================================================

project(${APPLICATION_NAME})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # Append these flags to the already existing CXX flags.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lpthread -pthread -Wall -g")
endif()

#==========================================
# Project Summary
#==========================================

message(STATUS "vvv Project Summary vvv\n")
message(STATUS "Name: ${APPLICATION_NAME}\n")

function(LIST_ITEMS files_list summary_heading)
  if(files_list)
    message(STATUS ${summary_heading})
    foreach(ITEM ${files_list})
      message(STATUS "> ${ITEM}")
    endforeach()
    message(STATUS "\n")
  endif()
endfunction()

LIST_ITEMS("${SRC_FILES}" "[Source Files]")
LIST_ITEMS("${SRC_HEADERS}" "[Source Headers]")
LIST_ITEMS("${RESOURCES}" "[Resources]")

#==========================================
# Macros
#==========================================

# Copy all 
macro(COPY_RESOURCES)
  if(WIN32 OR UNIX AND NOT APPLE)
      foreach(ITEM ${RESOURCES})
          string(LENGTH ${RESOURCE_DIR} ITEM_POSITION)
          string(LENGTH ${ITEM} ITEM_LENGTH)
          string(SUBSTRING ${ITEM} ${ITEM_POSITION} -1 TRIMMED_ITEM)
          add_custom_command(
              TARGET ${APPLICATION_NAME} POST_BUILD
              COMMAND ${CMAKE_COMMAND} -E copy ${ITEM} ${EXECUTABLE_OUTPUT_PATH}${TRIMMED_ITEM}
          )
      endforeach()
  endif()
endmacro()

#==========================================
# Compilation
#==========================================

if(UNIX AND NOT APPLE)

  # Create Project executable
  set(EXECUTABLE_OUTPUT_PATH ${OUT_DIR})
  set(CPACK_PROJECT_NAME ${PROJECT_NAME})
  set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
  add_executable(${APPLICATION_NAME} ${SRC_FILES} ${SRC_HEADERS})
  set_target_properties(${APPLICATION_NAME} PROPERTIES LINKER_LANGUAGE CXX)
  include_directories(${APPLICATION_NAME} ${SOURCE_INCLUDE_DIR})
  
  COPY_RESOURCES()

  # Raylib
  include_directories(${APPLICATION_NAME} 
    ${CMAKE_SOURCE_DIR}/lib/raylib/include
  )
  file(GLOB_RECURSE RAYLIB_H 
    "${CMAKE_SOURCE_DIR}/lib/raylib/include/*.h" 
  )
  LIST_ITEMS("${RAYLIB_H}" "[Raylib Headers]")
  target_sources(${APPLICATION_NAME} PRIVATE ${RAYLIB_H})
  target_link_libraries(${APPLICATION_NAME} ${CMAKE_SOURCE_DIR}/lib/raylib/libraylib.a)

  # Raygui
  include_directories(${APPLICATION_NAME} 
    ${CMAKE_SOURCE_DIR}/lib/raygui/include
  )
  file(GLOB_RECURSE RAYGUI_H 
    "${CMAKE_SOURCE_DIR}/lib/raygui/include/*.h" 
  )
  LIST_ITEMS("${RAYGUI_H}" "[Raygui Headers]")
  target_sources(${APPLICATION_NAME} PRIVATE ${RAYGUI_H})

  # AudioFile
  include_directories(${APPLICATION_NAME} 
  ${CMAKE_SOURCE_DIR}/lib/AudioFile
  )
  file(GLOB_RECURSE AUDIOFILE_H 
    "${CMAKE_SOURCE_DIR}/lib/AudioFile/*.h" 
  )
  LIST_ITEMS("${AUDIOFILE_H}" "[AudioFile Headers]")
  target_sources(${APPLICATION_NAME} PRIVATE ${AUDIOFILE_H})

endif()
