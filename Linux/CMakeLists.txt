cmake_minimum_required(VERSION 3.12)
project(Albus_Box_Linux CXX)

if (NOT CMAKE_CONFIGURATION_TYPES AND 
    NOT CMAKE_NO_BUILD_TYPE AND
    NOT CMAKE_BUILD_TYPE AND
    CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(STATUS "[!] Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

add_executable(AlbusBox
  Main.cpp
  Specific.hpp
  Specific.cpp
  ../src/Structs.hpp
  ../src/Player.hpp
  ../src/Player.cpp
  ../src/OtherFunctions.hpp
  ../src/OtherFunctions.cpp
  ../src/NeumorphismUI.hpp
  ../src/NeumorphismUI.cpp
  ../src/Header.hpp
  ../src/Enums.hpp
  ../src/ButtonEx.hpp
  ../src/Lyrics.hpp
  ../src/Lyrics.cpp
  ../src/AlbusBox.hpp
  ../src/AlbusBox.cpp
  )

find_package(Siv3D)
target_include_directories(AlbusBox PRIVATE 
"${PROJECT_SOURCE_DIR}"
"${PROJECT_SOURCE_DIR}/../src/"
)
target_link_libraries(AlbusBox PUBLIC Siv3D::Siv3D)

target_compile_features(AlbusBox PRIVATE cxx_std_20)

if(BUILD_TESTING)
enable_testing()
add_test(
  NAME Test
  COMMAND Siv3DTest
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
endif()
