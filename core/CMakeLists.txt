include(${PROJECT_SOURCE_DIR}/core/src.cmake)

include_directories("${PROJECT_SOURCE_DIR}/core")	
include_directories("${PROJECT_SOURCE_DIR}/core/license")
include_directories("${PROJECT_BINARY_DIR}")	
add_definitions(-DUS_RECON_CORE_SHARED_LIBS_EXPORT)
add_definitions(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
add_definitions(-D_UNICODE -DUNICODE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP") 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251") # disable warning 4251: dll-interface

add_library(us_recon SHARED ${core_src})
target_link_libraries(us_recon 
Shlwapi.lib
)

#install the project
install(FILES ${core_header}  DESTINATION include)
install(TARGETS us_recon
        RUNTIME DESTINATION bin/$<CONFIG> 
        LIBRARY DESTINATION lib/$<CONFIG> 
        ARCHIVE DESTINATION lib/$<CONFIG>)
		
#install(FILES "${PROJECT_OUTPUT_DIR}/debug/us_recond.lib" DESTINATION lib/debug)
#install(FILES "${PROJECT_OUTPUT_DIR}/release/us_recon.lib" DESTINATION lib/release)
#install(FILES "${PROJECT_OUTPUT_DIR}/debug/us_recond.dll" DESTINATION bin/debug)
#install(FILES "${PROJECT_OUTPUT_DIR}/release/us_recon.dll" DESTINATION bin/release)
#install(FILES "${PROJECT_SOURCE_DIR}/core/us_recon.cmake" DESTINATION .)

