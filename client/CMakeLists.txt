cmake_minimum_required(VERSION 3.5)
project(client)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED COMPONENTS system thread log)
find_package(vsomeip3 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${VSOMEIP_INCLUDE_DIRS})

add_executable(client client.cpp)

target_link_libraries(client
    ${Boost_LIBRARIES}
    vsomeip3
    vsomeip3-cfg
    vsomeip3-sd
)
