project(ProtocolBenchmarks)

find_package(benchmark REQUIRED)

get_filename_component(ProtocolBenchmarksDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(ProtocolBenchmarksSources
    ${ProtocolBenchmarksDir}/Main.cpp
)

add_executable(${PROJECT_NAME} ${ProtocolBenchmarksSources})

target_link_libraries(${PROJECT_NAME}
PUBLIC
    Protocol
    benchmark::benchmark
)
