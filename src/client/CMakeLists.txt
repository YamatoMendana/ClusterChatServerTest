aux_source_directory(. SRC_LIST)
aux_source_directory(./public PUBLIC_LIST)

add_executable(chatclient ${SRC_LIST} ${PUBLIC_LIST})

target_link_libraries(chatclient pthread jsoncpp )