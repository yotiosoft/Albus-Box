cmake_minimum_required(VERSION 3.12)
project(Albus_Box_Linux CXX C)

set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -fPIC -msse4.1 -D_GLFW_X11")
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -pg -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG -march=x86-64")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=x86-64")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -Wall -Wextra -Wno-unknown-pragmas -fPIC -msse4.1 -D_GLFW_X11")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -pg -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -march=x86-64")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=x86-64")

if (NOT CMAKE_CONFIGURATION_TYPES AND 
    NOT CMAKE_NO_BUILD_TYPE AND
    NOT CMAKE_BUILD_TYPE AND
    CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(STATUS "[!] Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options ("-fcolor-diagnostics")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options ("-fdiagnostics-color=always")
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
add_executable(Albus-Box
  Main.cpp
  Specific.hpp
  Specific.cpp
  ${PROJECT_SOURCE_DIR}/../../src/Structs.hpp
  ${PROJECT_SOURCE_DIR}/../../src/Player.hpp
  ${PROJECT_SOURCE_DIR}/../../src/Player.cpp
  ${PROJECT_SOURCE_DIR}/../../src/OtherFunctions.hpp
  ${PROJECT_SOURCE_DIR}/../../src/OtherFunctions.cpp
  ${PROJECT_SOURCE_DIR}/../../src/NeumorphismUI.hpp
  ${PROJECT_SOURCE_DIR}/../../src/NeumorphismUI.cpp
  ${PROJECT_SOURCE_DIR}/../../src/Header.hpp
  ${PROJECT_SOURCE_DIR}/../../src/Enums.hpp
  ${PROJECT_SOURCE_DIR}/../../src/ButtonEx.hpp
  ${PROJECT_SOURCE_DIR}/../../src/Lyrics.hpp
  ${PROJECT_SOURCE_DIR}/../../src/Lyrics.cpp
  ${PROJECT_SOURCE_DIR}/../../src/AlbusBox.hpp
  ${PROJECT_SOURCE_DIR}/../../src/AlbusBox.cpp
  )
target_include_directories(Albus-Box PRIVATE
  "/usr/include"
  "~/OpenSiv3D/Siv3D/include"
  "~/OpenSiv3D/Siv3D/include/ThirdParty"
  "${PROJECT_SOURCE_DIR}/../../src/"
  "${PROJECT_SOURCE_DIR}"
  )
target_link_libraries(Albus-Box PRIVATE
  ${PROJECT_SOURCE_DIR}/../build/libSiv3D.a
  -lasound
  -lavcodec
  -lavformat
  -lavutil
  -lcurl
  -ldl
  -lfreetype
  -lGL
  -lglib-2.0
  -lgobject-2.0
  -lgif
  -lgio-2.0
  -lgdk-3
  -lgtk-3
  -lharfbuzz
	-logg
  -lopencv_core
  -lopencv_imgcodecs
  -lopencv_imgproc
  -lopencv_objdetect
  -lopencv_photo
  -lopencv_videoio
  -lopus
  -lopusfile
  -lpthread
  -lpng
  -lSoundTouch
  -lswresample
  -ltiff
  -lturbojpeg
  -luuid
	-lvorbis
	-lvorbisenc
	-lvorbisfile
  -lwebp
  -lX11
  -lXft
  )


if(BUILD_TESTING)
enable_testing()
add_test(
  NAME Test
  COMMAND Albus-Box
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
endif()
