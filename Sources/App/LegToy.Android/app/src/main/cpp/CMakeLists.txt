cmake_minimum_required(VERSION 3.18.1)

# Project App
project("LegToy")

add_library(LegToy SHARED
        jni/Android.cpp
        LegToyAndroidSetting.h LegToyAndroidSetting.cpp
        LegToyAndroid.cpp
)


# Android system library
find_library(nlib-z z)
find_library(nlib-log log)
find_library(nlib-android android)
find_library(nlib-GLESv3 GLESv3)
find_library(nlib-EGL EGL)
find_library(nlib-OpenSLES OpenSLES)

# App library
find_package(oboe REQUIRED CONFIG)

# Leggiero
add_subdirectory("../../../../../../../../Engine" "../../../../../../../../.Build/Android/Leggiero")


# Project Logic
add_subdirectory("../../../../../../LegToy" "../../../../../../../../.Build/LegToy")
add_dependencies(LegToy_Game Leggiero)


target_link_libraries(LegToy
        ${nlib-z} ${nlib-log} ${nlib-android} ${nlib-GLESv3} ${nlib-EGL} ${nlib-OpenSLES}
        oboe::oboe
        Leggiero
        LegToy_Game
)