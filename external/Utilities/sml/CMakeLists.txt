# Samson math library
samson_define_module(sml) 

target_link_libraries(${PROJECT_NAME} PUBLIC sio.logger)

add_subdirectory(test)