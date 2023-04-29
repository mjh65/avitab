if(WIN32 AND DEFINED ENV{MSFS_SDK})
include(${CMAKE_CURRENT_LIST_DIR}/panel/CMakeLists.txt)
include_directories("$ENV{MSFS_SDK}/SimConnect SDK/include")
target_sources(avitab_msfs_igps PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/IngamePanelEnvironment.cpp
    ${CMAKE_CURRENT_LIST_DIR}/IngamePanelGuiDriver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/IngamePanelServer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/IngamePanelHttpReq.cpp
)
endif()
