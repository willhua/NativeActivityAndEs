
cmake_minimum_required(VERSION 3.4.1)


add_library( main

             SHARED

             src/main/cpp/MyNativeMain.cpp )


find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

target_link_libraries( # Specifies the target library.
#    重要：：：不能命名为native-main, 否则会出现Unable to find native library main using classloader:的错误。应该是不能以native开头
#    改成nativemain2, anativemain2 都不行，貌似只能用main?
#
                       main

                       ${log-lib} )