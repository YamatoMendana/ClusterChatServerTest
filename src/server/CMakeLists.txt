aux_source_directory(. SRC_LIST)
aux_source_directory(./database DB_LIST)
aux_source_directory(./model MODEL_LIST)
aux_source_directory(./public PUBLIC_LIST)
aux_source_directory(./redis REDIS_LIST)

add_executable(chatserver ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${PUBLIC_LIST} ${REDIS_LIST})

target_link_libraries(chatserver muduo_net muduo_base mysqlclient pthread jsoncpp hiredis)

