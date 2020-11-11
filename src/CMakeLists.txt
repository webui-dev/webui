
# Microsoft build tools
if (MSVC)
    add_link_options("/SUBSYSTEM:WINDOWS")
endif (MSVC)

# Build WebUI Library
include_directories(${WebUIProject_SOURCE_DIR}/thirdparty/boost/include)
add_library(webui STATIC webui.cpp)
set_target_properties(webui PROPERTIES OUTPUT_NAME webui${bit}) # Trying to set webuix32 / webuix64 but not working!
