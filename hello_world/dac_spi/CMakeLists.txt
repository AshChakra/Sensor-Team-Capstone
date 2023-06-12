cmake_minimum_required(VERSION 3.0)
project(dac_spi)

# Set the C standard (change to your desired version if needed)
set(CMAKE_C_STANDARD 11)

# Add your source files here
set(SOURCES
    test.c
    orin_spi.c
    # Add more source files as needed
)

# Add your header files here
set(HEADERS
    orin_spi.h
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
