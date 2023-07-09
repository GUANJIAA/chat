# 定义了一个SRC_LIST变量，包含了该目录下所有源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./mysqldb DB_LIST)
# aux_source_directory(./model MODEL_LIST)
# aux_source_directory(./redis MODEL_LIST)

LINK_LIBRARIES("/root/projects/examples/chat_room/thirdparty/sylar/lib/libsylar.so")

#指定生成可执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_LIST})


#指定可执行文件链接时需要加载的依赖库
target_link_libraries(ChatServer
                        sylar
                        "/usr/lib64/mysql/libmysqlclient.so"
                        yaml-cpp
                        pthread)            