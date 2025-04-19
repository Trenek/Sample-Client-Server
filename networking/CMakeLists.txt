cmake_minimum_required(VERSION 3.10)

project(Client)

add_executable(Client
    "${CMAKE_CURRENT_SOURCE_DIR}/client.c"
)

project(Server)

add_executable(Server
    "${CMAKE_CURRENT_SOURCE_DIR}/server.c"
)
