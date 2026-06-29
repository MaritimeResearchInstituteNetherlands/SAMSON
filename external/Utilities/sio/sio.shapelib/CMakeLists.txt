samson_define_module(sio.shapelib)

find_package(shapelib CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC shapelib::shp)

samson_install_dependencies(${PROJECT_NAME})