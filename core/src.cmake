set(dir "${PROJECT_SOURCE_DIR}/core")

file(GLOB core_header
    "${dir}/*.h"
)
file(GLOB core_src
    "${dir}/*.cpp"
)
# 将头文件和源文件分组
#source_group("Header Files" FILES ${core_header})
#source_group("Source Files" FILES ${core_src})

set(core_src ${core_header} ${core_src})
