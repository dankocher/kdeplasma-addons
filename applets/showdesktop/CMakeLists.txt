plasma_install_package(package org.kde.plasma.showdesktop)

add_definitions(-DTRANSLATION_DOMAIN="plasma_applet_org.kde.plasma.showdesktop")

set(showdesktop_SRCS
    plugin/showdesktop.cpp
    plugin/showdesktopplugin.cpp
)

install(FILES plugin/qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/showdesktop)
add_library(showdesktopplugin SHARED ${showdesktop_SRCS})

target_link_libraries(showdesktopplugin
     Qt5::Core
     Qt5::Qml
     Qt5::Quick
     KF5::KDELibs4Support
    )

if (Qt5X11Extras_FOUND)
    target_link_libraries(showdesktopplugin
         Qt5::X11Extras
        )
endif()

install(TARGETS showdesktopplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/showdesktop)
