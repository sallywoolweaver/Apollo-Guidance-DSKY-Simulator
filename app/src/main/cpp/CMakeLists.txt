# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.22.1)

project("myapplication")

add_library(
    myapplication
    SHARED
    AlarmExecutive.cpp
    AgcCpu.cpp
    AgcMemoryImage.cpp
    CompatibilityScenario.cpp
    DskyIo.cpp
    NativeApolloCore.cpp
    NativeApolloJni.cpp
    ScenarioBootstrap.cpp
    SnapshotBuilder.cpp
)

target_link_libraries(
    myapplication
    android
    log
)
