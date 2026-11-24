# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Pull in the ola-home firmware module (firmware/) without relying on west
# module discovery.
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_LIST_DIR}/../..)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lamp)

target_sources(app PRIVATE
  src/main.c
  src/lamp_input.c
)
