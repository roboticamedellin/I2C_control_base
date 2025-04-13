cmake_minimum_required(VERSION 3.10)

# Project name
project(I2C_control_base)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executables
add_executable(read_encoder src/read_encoder.cpp)
add_executable(send_command src/send_command.cpp)
add_executable(stream_thread src/stream_thread.cpp)

# Link libraries if needed (e.g., pthread and i2c)
target_link_libraries(read_encoder i2c)
target_link_libraries(send_command i2c)
target_link_libraries(stream_thread pthread i2c)
