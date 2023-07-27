cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(capstone C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Link project files
add_executable(testbed
	main.c
	accelerometer.c
	lcd.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(testbed
	pico_stdlib
	hardware_spi
)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(testbed)

# enable usb output, disable uart output
pico_enable_stdio_usb(testbed 1)
pico_enable_stdio_uart(testbed 0)
