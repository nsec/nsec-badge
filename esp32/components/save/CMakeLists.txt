cmake_minimum_required(VERSION 3.5)

idf_component_register(
  SRCS
      "save.cpp"
  INCLUDE_DIRS
      "."
  REQUIRES
      nvs_flash
)
