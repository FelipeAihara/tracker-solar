add_library(servo_lib STATIC)
target_sources(servo_lib PRIVATE src/servo.cpp)
target_include_directories(servo_lib PUBLIC include)
target_link_libraries(servo_lib PUBLIC pwm_lib PUBLIC zephyr_interface)

# FORÇA O CMAKE A ESPERAR OS ARQUIVOS GERADOS DO ZEPHYR ANTES DE COMPILAR O SERVO
add_dependencies(servo_lib zephyr_generated_headers)