cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(Protocol)

get_filename_component(ProtocolRoot ${CMAKE_CURRENT_LIST_FILE} PATH)

include(${ProtocolRoot}/CompileOptions/CompileOptions.cmake)

include(${ProtocolRoot}/Protocol/Protocol.cmake)

if(TESTS)
    enable_testing()
    include(${ProtocolRoot}/Tests/ProtocolTests.cmake)
endif()

if(BENCHMARKS)
    include(${ProtocolRoot}/Benchmarks/ProtocolBenchmarks.cmake)
endif()