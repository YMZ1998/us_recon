set(test_src main.cpp)

include_directories("${PROJECT_SOURCE_DIR}/test")
include_directories("${PROJECT_SOURCE_DIR}/core")

add_compile_options("/std:c++17")

add_definitions(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
add_definitions(-D_UNICODE -DUNICODE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251") # disable warning 4251: dll-interface
add_executable(us_recon_test ${test_src})

target_link_libraries(us_recon_test
us_recon
)