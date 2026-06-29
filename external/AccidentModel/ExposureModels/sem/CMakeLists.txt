# Samson exposure model
samson_define_module(sem)

#BOOST
set(Boost_NO_WARN_NEW_VERSIONS 1)
find_package(Boost 1.83.0 REQUIRED 
 COMPONENTS serialization 
)
# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
# Link Boost libraries
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})


target_link_libraries(${PROJECT_NAME} PUBLIC sim tdb yaml-cpp::yaml-cpp)

add_subdirectory(test)

