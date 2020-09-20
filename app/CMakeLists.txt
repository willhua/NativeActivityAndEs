
cmake_minimum_required(VERSION 3.4.1)

#target_compile_features(main PRIVATE cxx_range_for)
#set(CMAKE_CXX11_EXTENSION_COMPILE_OPTION "-std=c++17")
#set(CMAKE_CXX_FLAGS "-std=c++11")
#set(ANDROID_STL "c++_static")
#set(OpenCV_DIR E:\\opencv_345_android_sdk\\OpenCV-android-sdk\\sdk\\native\\jni)
#find_package(OpenCV REQUIRED)

#如果当前系统版本过低设置拒绝编译
if (${ANDROID_PLATFORM_LEVEL} LESS 11)
  message(FATAL_ERROR "OpenGL 2 is not supported before API level 11 (currently using ${ANDROID_PLATFORM_LEVEL}).")
  return()
elseif (${ANDROID_PLATFORM_LEVEL} LESS 18)
  add_definitions("-DDYNAMIC_ES3")
  set(OPENGL_LIB GLESv2)
else ()
  set(OPENGL_LIB GLESv3)
endif (${ANDROID_PLATFORM_LEVEL} LESS 11)


aux_source_directory(src/main/cpp CPP_SRC)

add_library( main
             SHARED

             src/main/cpp/stb/stb_image.cpp
             ${CPP_SRC} )


find_library( log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

target_link_libraries( # Specifies the target library.
#    重要：：：不能命名为native-main, 否则会出现Unable to find native library main using classloader:的错误。应该是不能以native开头
#    改成nativemain2, anativemain2 都不行，貌似只能用main?
#
                       main
                       ${log-lib}
                       android
                       jnigraphics
                       ${OPENGL_LIB}
                       EGL
                       log
                       m
                       ${OpenCV_LIBS})