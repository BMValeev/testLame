cmake_minimum_required(VERSION 3.8)
project(testLame)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(BUILD_SHARED_LIBS OFF)

set(SOURCE_FILES_PRJ
        main.cpp
        coder/Coder.cpp
        coder/CoderQueue.cpp
        baseClasses/QueueBase.cpp
        baseClasses/WorkerBase.cpp
        audio/mp3Encoder.cpp
        audio/wavReader.cpp
        platformDependent/folderReader.cpp
        )

list(APPEND DIRECTORIES_PRJ        "libmp3lame-CMAKE"        )
list(APPEND LIBRARIES_PRJ            mp3lame         )

find_package(Threads REQUIRED)
list(APPEND LIBRARIES_PRJ        Threads::Threads        )
if(WIN32 OR WIN64)
    message("Windows build")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.dll;")
endif()
if(UNIX)
    message("Linux build")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;")
endif()
add_subdirectory("libmp3lame-CMAKE")
link_directories(${DIRECTORIES_PRJ})
add_executable(${PROJECT_NAME} ${SOURCE_FILES_PRJ})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES_PRJ})