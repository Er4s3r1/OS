set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.22)
project(os_kp)

# Добавьте заголовочные файлы cppzmq
include_directories(${CMAKE_SOURCE_DIR}/cppzmq)

# Добавьте директорию, содержащую библиотеки ZeroMQ
link_directories(${CMAKE_SOURCE_DIR}/cppzmq)

add_compile_options(-pthread)

add_executable(client ./client.cpp)
add_executable(server ./server.cpp)
add_executable(game ./game.cpp)

# Добавьте исходные файлы cppzmq
target_sources(client PRIVATE ${CMAKE_SOURCE_DIR}/cppzmq/zmq.hpp)
target_sources(server PRIVATE ${CMAKE_SOURCE_DIR}/cppzmq/zmq.hpp)
target_sources(game PRIVATE ${CMAKE_SOURCE_DIR}/cppzmq/zmq.hpp)

# Линковка с библиотекой ZeroMQ
target_link_libraries(client PRIVATE zmq)
target_link_libraries(server PRIVATE zmq)
target_link_libraries(game PRIVATE zmq)