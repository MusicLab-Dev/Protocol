cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(Protocol)

get_filename_component(ProtocolDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(ProtocolSources
    ${ProtocolDir}/Protocol.hpp
    ${ProtocolDir}/Packet.hpp
    ${ProtocolDir}/Packet.ipp
    ${ProtocolDir}/Packet.cpp
)

add_library(${PROJECT_NAME} ${ProtocolSources})

target_include_directories(${PROJECT_NAME} PUBLIC ${ProtocolDir}/..)

target_link_libraries(${PROJECT_NAME} PUBLIC Core)

if(CODE_COVERAGE)
    target_compile_options(${PROJECT_NAME} PUBLIC --coverage)
    target_link_options(${PROJECT_NAME} PUBLIC --coverage)
endif()