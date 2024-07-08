cmake_minimum_required(VERSION 3.24)
project(zsnapper)

set(CMAKE_CXX_STANDARD 23)

find_package(nlohmann_json)

add_executable(zsnapper main.cpp
        ZsnapperConfig.cpp
        ZsnapperConfig.h
        ZsnapperLoop.cpp
        ZsnapperLoop.h
        ZsnapperState.cpp
        ZsnapperState.h
        ZfsDataset.cpp
        ZfsDataset.h
        SystemExec.cpp
        SystemExec.h
        Fork.cpp
        Fork.h
        ExecGetOutput.cpp
        ExecGetOutput.h
        ZfsSnapshot.cpp
        ZfsSnapshot.h
        SimpleExec.cpp
        SimpleExec.h)

target_link_libraries(zsnapper PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(zsnapper PRIVATE -lpthread)

IF(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    target_include_directories(zsnapper PRIVATE /usr/local/include)
ENDIF(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")