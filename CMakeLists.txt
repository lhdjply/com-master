cmake_minimum_required(VERSION 3.16)

# 使用变量定义项目名/目标名
set(PROJECT_TARGET ly-serial-tool)

project(${PROJECT_TARGET} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort LinguistTools)

set (LY_QRC_FILES
    assets/resources.qrc
)

qt6_standard_project_setup(
    I18N_TRANSLATED_LANGUAGES en zh_CN
)

# 自动查找源文件
file(GLOB_RECURSE PROJECT_SOURCES
    "include/*.h"
    "myresource/*.h"
    "myresource/*.cpp"
    "myresource/*.ui"
    "src/*.cpp"
)

include_directories(include)
include_directories(myresource/inc)

# 使用变量创建目标
qt_add_executable(${PROJECT_TARGET}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${LY_QRC_FILES}
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

# 使用变量安装
include(GNUInstallDirs)
install(TARGETS ${PROJECT_TARGET}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (WIN32)

elseif (APPLE)

elseif (UNIX)
install(FILES ${CMAKE_SOURCE_DIR}/ly-serial-tool.desktop
        DESTINATION share/applications
)
endif()


# 使用变量完成配置
qt_finalize_executable(${PROJECT_TARGET})
