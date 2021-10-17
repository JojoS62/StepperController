# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)

set(MBED_PATH ${CMAKE_CURRENT_SOURCE_DIR}/mbed-os CACHE INTERNAL "")
set(MBED_CONFIG_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
set(APP_TARGET StepperController)

include(${MBED_PATH}/tools/cmake/app.cmake)

project(${APP_TARGET})

add_subdirectory(${MBED_PATH})
add_subdirectory(custom_targets)

set(MICROPY_DIR "dummy to force lvgl to use static lib")
add_subdirectory(lvgl)
add_subdirectory(lvglDriver)

target_include_directories(lvglDriver INTERFACE lvgl)

set(SOURCES
    src/globals.cpp
    src/HWTimer.cpp
    src/StepperController.cpp
    src/MotionController.cpp
    lvScreens/lvParameterScreen.cpp
    lvScreens/lvSplashScreen.c
)

add_executable(${APP_TARGET}
    main.cpp
    ${SOURCES}
)

target_link_libraries(${APP_TARGET}
    PRIVATE
        mbed-os
        mbed-events
        lvgl_interface
        lvglDriver
)

mbed_set_post_build(${APP_TARGET})

option(VERBOSE_BUILD "Have a verbose build process")
if(VERBOSE_BUILD)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()