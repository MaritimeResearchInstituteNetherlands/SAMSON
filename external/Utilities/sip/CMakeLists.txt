# Samson input processor
samson_define_module(sip) 

find_package(yaml-cpp REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC tdb tdb.shapelib sim sim.shapelib sim.csv sio.logger sem yaml-cpp::yaml-cpp)

samson_install_dependencies(${PROJECT_NAME})