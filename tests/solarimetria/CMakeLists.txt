cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(test_solarimetria)

set(APP_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)

target_sources(app PRIVATE
    src/test_solarimetria.cpp
    ${APP_ROOT}/lib/solarimetria/src/solarimetria.cpp
)

target_include_directories(app PRIVATE
    ${APP_ROOT}/lib/solarimetria/include
    ${APP_ROOT}/lib/rtc/include
)
