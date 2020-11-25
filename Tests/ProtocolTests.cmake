project(ProtocolTests)

find_package(GTest REQUIRED)

get_filename_component(ProtocolTestsDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(ProtocolTestsSources
    ${ProtocolTestsDir}/tests_Packet.cpp
)

add_executable(${PROJECT_NAME} ${ProtocolTestsSources})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
PUBLIC
    Protocol
    GTest::GTest GTest::Main
)