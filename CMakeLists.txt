cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(tracker_solar)

target_sources(app PRIVATE
    src/main.cpp

    lib/pot/src/pot.cpp
    # lib/pwm_frdm/src/pwm_z42.c
    # lib/servo_frdm/src/servo_frdm.cpp
    lib/rtc/src/rtc.cpp
    lib/tracker/src/tracker.cpp
    lib/lcd/src/lcd.cpp
    lib/button/src/button.cpp
)

target_include_directories(app PRIVATE
    lib/pot/include
    # lib/pwm_frdm/include
    # lib/servo_frdm/include
    lib/rtc/include
    lib/tracker/include
    lib/lcd/include
    lib/button/include
)
