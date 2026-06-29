samson_define_module(tdb.shapelib)

#target_include_directories(${PROJECT_NAME} PRIVATE)
target_link_libraries(${PROJECT_NAME} PUBLIC sim sio.shapelib)


