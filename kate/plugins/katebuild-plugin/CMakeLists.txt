# building separately or as part of kdesdk?
if(NOT KDE4_FOUND)
    find_package(KDE4 REQUIRED)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif(NOT KDE4_FOUND)

include_directories (${KDE4_INCLUDES})
include_directories( ${KDE4_KIO_INCLUDES} )

set(katebuild_SRCS plugin_katebuild.cpp targets.cpp)

kde4_add_ui_files(katebuild_SRCS build.ui)

kde4_add_plugin(katebuildplugin ${katebuild_SRCS})

target_link_libraries(katebuildplugin ${KDE4_KDECORE_LIBS} kateinterfaces ${KDE4_KTEXTEDITOR_LIBS} )

install(TARGETS katebuildplugin DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES ui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kate/plugins/katebuild )
install( FILES katebuildplugin.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
