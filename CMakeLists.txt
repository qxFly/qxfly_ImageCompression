cmake_minimum_required(VERSION 3.29)
project(qxfly_ImageCompression)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
#设置CMAKE前缀路径
set(CMAKE_PREFIX_PATH "F:/Code_Software/Qt/6.6.3/mingw_64/lib/cmake")
#设置OpenCV库路径
set(OpenCV_DIR "F:/Code_Software/OpenCV/build_mingw/x64/mingw/lib")
#查找OpenCV库
find_package(OpenCV REQUIRED)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        Concurrent
        REQUIRED)
#设置qrc文件路径
set(QRC_SOURCE_FILES resources/rec.qrc)
#为qt添加qrc文件
qt_add_resources(${QRC_SOURCE_FILES})
#包含头文件
include_directories(
        src/widgets
        src/models
        src/utils
        ${OpenCV_INCLUDE_DIRS}# OpenCV_INCLUDE_DIRS 是预定义变量，代表OpenCV库的头文件路径
)
#链接动态库
link_libraries()
#可执行文件
add_executable(qxfly_ImageCompression
#        WIN32
        main.cpp
        src/widgets/mainWidget.cpp
        src/widgets/ImageGraphicsView.cpp
        src/widgets/CITableWidgetItem.cpp
        src/utils/Utils.h
        src/models/CompressionImage.cpp
        src/models/TableItemData.cpp
        ${QRC_SOURCE_FILES}
)
#链接库
target_link_libraries(qxfly_ImageCompression
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::Concurrent
        ${OpenCV_LIBS}# OpenCV_LIBS 是预定义变量，代表OpenCV库的lib库文件
)
if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
if (EXISTS "${QT_INSTALL_PATH}/plugins/styles/qwindowsvistastyle${DEBUG_SUFFIX}.dll")
    add_custom_command(TARGET qxfly_ImageCompression POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:qxfly_ImageCompression>/plugins/styles/")
    add_custom_command(TARGET qxfly_ImageCompression POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${QT_INSTALL_PATH}/plugins/styles/qwindowsvistastyle${DEBUG_SUFFIX}.dll"
            "$<TARGET_FILE_DIR:qxfly_ImageCompression>/plugins/styles/")
endif ()
add_custom_command(TARGET qxfly_ImageCompression POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:qxfly_ImageCompression>/imageformats/")

# qgif qico qjpeg qsvg
FILE(GLOB IMAGE_DLLS "${QT_INSTALL_PATH}/plugins/imageformats/*${DEBUG_SUFFIX}.dll")
foreach (IMAGE_DLL ${IMAGE_DLLS})
    add_custom_command(TARGET qxfly_ImageCompression POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${IMAGE_DLL}"
            "$<TARGET_FILE_DIR:qxfly_ImageCompression>/imageformats/")
endforeach (IMAGE_DLL)