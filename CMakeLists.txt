cmake_minimum_required(VERSION 3.16)

# 使用变量定义项目名/目标名
set(PROJECT_TARGET ly-serial-tool)

project(${PROJECT_TARGET} VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加编译器标志以禁用特定警告
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-stringop-overflow")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-stringop-overflow")

# 添加RC语言支持
if(WIN32)
    enable_language(RC)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort LinguistTools)

set (LY_QRC_FILES
    assets/resources.qrc
)

qt6_standard_project_setup(
    I18N_TRANSLATED_LANGUAGES en zh_CN
)

# Windows图标资源文件
if(WIN32)
    set(WIN_RESOURCES "${CMAKE_SOURCE_DIR}/assets/icons/windows/win_resources.rc")
endif()

# 自动查找源文件
file(GLOB_RECURSE PROJECT_SOURCES
    "include/*.h"
    "myresource/*.h"
    "third_party/libiec61850/*.h"
    "myresource/*.cpp"
    "src/*.cpp"
    "third_party/qtermwidget/*.cpp"
    "third_party/libiec61850/*.c"
)

include_directories(include)
include_directories(myresource/inc)
include_directories(third_party/qtermwidget)
include_directories(third_party/libiec61850/include)
include_directories(third_party/libiec61850/source/src/common/inc)
include_directories(third_party/libiec61850/source/src/goose)
include_directories(third_party/libiec61850/source/src/sampled_values)
include_directories(third_party/libiec61850/source/src/r_session)
include_directories(third_party/libiec61850/source/src/hal/inc)
include_directories(third_party/libiec61850/source/src/iec61850/inc)
include_directories(third_party/libiec61850/source/src/iec61850/inc_private)
include_directories(third_party/libiec61850/source/src/mms/inc)
include_directories(third_party/libiec61850/source/src/mms/inc_private)
include_directories(third_party/libiec61850/source/src/mms/iso_mms/asn1c)
include_directories(third_party/libiec61850/source/src/logging)

# 定义TRANSLATIONS_DIR宏、KB_LAYOUT_DIR宏和COLORSCHEMES_DIR宏
add_definitions(-DKB_LAYOUT_DIR="${CMAKE_SOURCE_DIR}/third_party/qtermwidget/kb-layouts")
add_definitions(-DCOLORSCHEMES_DIR="${CMAKE_SOURCE_DIR}/third_party/qtermwidget/color-schemes")
add_definitions(-DKONSOLE_DSHELL=1)
add_definitions(-DHAVE_POSIX_OPENPT=1)

# 使用变量创建目标
qt_add_executable(${PROJECT_TARGET}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${LY_QRC_FILES}
    ${WIN_RESOURCES}
)

# 使用变量添加翻译
qt6_add_translations(${PROJECT_TARGET}
    TS_FILE_DIR myresource/src/translations
)

# 使用变量链接库
target_link_libraries(${PROJECT_TARGET} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::SerialPort)

# 使用变量设置属性
set_target_properties(${PROJECT_TARGET} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Windows特定设置
if(WIN32)
    # 设置Windows可执行文件图标
    set_target_properties(${PROJECT_TARGET} PROPERTIES
        RC_ICONS "${CMAKE_SOURCE_DIR}/assets/icons/windows/ly-serial-tool.ico"
    )
    
elseif (APPLE)
    
elseif (UNIX)

    set(ICON16 assets/icons/linux/hicolor/16x16/apps/${PROJECT_TARGET}.png )
    set(ICON22 assets/icons/linux/hicolor/22x22/apps/${PROJECT_TARGET}.png )
    set(ICON32 assets/icons/linux/hicolor/32x32/apps/${PROJECT_TARGET}.png )
    set(ICON48 assets/icons/linux/hicolor/48x48/apps/${PROJECT_TARGET}.png )
    set(ICON64 assets/icons/linux/hicolor/64x64/apps/${PROJECT_TARGET}.png )
    set(ICON128 assets/icons/linux/hicolor/128x128/apps/${PROJECT_TARGET}.png )
    set(ICON256 assets/icons/linux/hicolor/256x256/apps/${PROJECT_TARGET}.png )
    set(ICON512 assets/icons/linux/hicolor/512x512/apps/${PROJECT_TARGET}.png )
    set(ICONsc assets/icons/linux/hicolor/scalable/apps/${PROJECT_TARGET}.svg )

    install( FILES ${ICON16} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/16x16/apps)
    install( FILES ${ICON22} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/22x22/apps)
    install( FILES ${ICON32} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/32x32/apps)
    install( FILES ${ICON48} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/48x48/apps)
    install( FILES ${ICON64} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/64x64/apps)
    install( FILES ${ICON128} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/128x128/apps)
    install( FILES ${ICON256} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/256x256/apps)
    install( FILES ${ICON512} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/512x512/apps)
    install( FILES ${ICONsc} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)

    install(FILES ${CMAKE_SOURCE_DIR}/assets/desktop/ly-serial-tool.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
endif()

# 使用变量安装
include(GNUInstallDirs)
install(TARGETS ${PROJECT_TARGET}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 使用变量完成配置
qt_finalize_executable(${PROJECT_TARGET})
