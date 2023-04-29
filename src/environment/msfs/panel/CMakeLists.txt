if(WIN32 AND DEFINED ENV{MSFS_SDK})
add_custom_command(
    OUTPUT
        avitab-ingamepanels-avitab/html_ui/icons/toolbar/ICON_TOOLBAR_AVITAB_PANEL.svg
        avitab-ingamepanels-avitab/html_ui/InGamePanels/AvitabPanel/AvitabPanel.css
        avitab-ingamepanels-avitab/html_ui/InGamePanels/AvitabPanel/AvitabPanel.html
        avitab-ingamepanels-avitab/html_ui/InGamePanels/AvitabPanel/AvitabPanel.js
        avitab-ingamepanels-avitab/html_ui/Textures/Menu/toolbar/ICON_TOOLBAR_AVITAB_PANEL.svg
        avitab-ingamepanels-avitab/InGamePanels/avitab-ingamepanels-avitab.spb
        avitab-ingamepanels-avitab/layout.json
        avitab-ingamepanels-avitab/manifest.json
    COMMAND 
        ${CMAKE_CURRENT_LIST_DIR}/build.sh
    DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/build.sh
        ${CMAKE_CURRENT_LIST_DIR}/html_ui/icons/toolbar/ICON_TOOLBAR_AVITAB_PANEL.svg
        ${CMAKE_CURRENT_LIST_DIR}/html_ui/InGamePanels/AvitabPanel/AvitabPanel.css
        ${CMAKE_CURRENT_LIST_DIR}/html_ui/InGamePanels/AvitabPanel/AvitabPanel.html
        ${CMAKE_CURRENT_LIST_DIR}/html_ui/InGamePanels/AvitabPanel/AvitabPanel.js
        ${CMAKE_CURRENT_LIST_DIR}/spb/avitab-ingamepanels-avitab.xml
        ${CMAKE_CURRENT_LIST_DIR}/spb/PackageDefinitions/avitab-ingamepanels-avitab.xml
        ${CMAKE_CURRENT_LIST_DIR}/spb/PackageSources/avitab-ingamepanels-avitab.xml
)

add_custom_target(
    Avitab-IngamePanel ALL
    DEPENDS
        avitab-ingamepanels-avitab/html_ui/icons/toolbar/ICON_TOOLBAR_AVITAB_PANEL.svg
        avitab-ingamepanels-avitab/html_ui/InGamePanels/AvitabPanel/AvitabPanel.css
        avitab-ingamepanels-avitab/html_ui/InGamePanels/AvitabPanel/AvitabPanel.html
        avitab-ingamepanels-avitab/html_ui/InGamePanels/AvitabPanel/AvitabPanel.js
        avitab-ingamepanels-avitab/html_ui/Textures/Menu/toolbar/ICON_TOOLBAR_AVITAB_PANEL.svg
        avitab-ingamepanels-avitab/InGamePanels/avitab-ingamepanels-avitab.spb
        avitab-ingamepanels-avitab/layout.json
        avitab-ingamepanels-avitab/manifest.json
)
endif()
