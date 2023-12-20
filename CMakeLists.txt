cmake_minimum_required(VERSION 3.27)
project(map_unordered_flat)

find_package(Boost REQUIRED)
find_package(benchmark REQUIRED)

add_executable(map_unordered_flat main.cpp)

target_link_libraries(map_unordered_flat benchmark::benchmark Boost::boost)
target_compile_features(map_unordered_flat PUBLIC cxx_std_17)
