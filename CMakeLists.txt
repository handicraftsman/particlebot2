cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(ParticleBot2 VERSION 0.1.0 LANGUAGES CXX)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX /usr/ CACHE PATH "Default install prefix" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif()

set(CMAKE_CXX_STANDARD 17)


find_package(PkgConfig REQUIRED)
pkg_search_module(Guosh REQUIRED guosh>=0.4.0)
pkg_search_module(PDI REQUIRED particledi>=0.3.0)
pkg_search_module(PINI REQUIRED particleini)
pkg_search_module(SQLite REQUIRED sqlite3)
set(PB2_LIBRARIES ${Guosh_LIBRARIES} ${PDI_LIBRARIES} ${PINI_LIBRARIES} ${SQLite_LIBRARIES} dl pthread)
set(PB2_INCLUDE_DIRS ${Guosh_INCLUDE_DIRS} ${PDI_INCLUDE_DIRS} ${PINI_INCLUDE_DIRS} ${SQLite_INCLUDE_DIRS})


file(GLOB pb2_SRC particlebot2/*.cpp)
add_library(libparticlebot2 SHARED ${pb2_SRC})
target_link_libraries(libparticlebot2 ${PB2_LIBRARIES})
target_include_directories(libparticlebot2 PRIVATE ${PB2_INCLUDE_DIRS})
set_target_properties(libparticlebot2 PROPERTIES OUTPUT_NAME particlebot2)

file(GLOB pb2_core_SRC core/*.cpp)
add_library(pb2-core SHARED ${pb2_core_SRC})
target_link_libraries(pb2-core libparticlebot2 ${PB2_LIBRARIES})
target_include_directories(pb2-core PRIVATE . ${PB2_INCLUDE_DIRS})

add_executable(particlebot2 main.cpp)
target_link_libraries(particlebot2 libparticlebot2 ${PB2_LIBRARIES})
target_include_directories(particlebot2 PRIVATE ${PB2_INCLUDE_DIRS})


configure_file(${CMAKE_SOURCE_DIR}/particlebot2.pc.in ${CMAKE_BINARY_DIR}/particlebot2.pc)
install(FILES ${CMAKE_BINARY_DIR}/particlebot2.pc DESTINATION lib/pkgconfig)


install(
  TARGETS particlebot2 libparticlebot2 pb2-core
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
)

install(FILES particlebot2.hpp DESTINATION include)
file(GLOB pb2_HDR particlebot2/*.hpp)
install(FILES ${pb2_HDR} DESTINATION include/particlebot2)