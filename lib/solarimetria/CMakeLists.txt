add_library(solarimetria_lib STATIC)
target_sources(solarimetria_lib PRIVATE src/solarimetria.cpp)
target_include_directories(solarimetria_lib PUBLIC include)
target_link_libraries(solarimetria_lib PUBLIC rtc_lib PUBLIC zephyr_interface)

# FORÇA O CMAKE A ESPERAR OS ARQUIVOS GERADOS DO ZEPHYR ANTES DE COMPILAR
add_dependencies(solarimetria_lib zephyr_generated_headers)
