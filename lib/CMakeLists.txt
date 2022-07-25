add_library(mul-to-bitshift SHARED
    MulToBitshift.cpp)
target_include_directories(mul-to-bitshift PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")