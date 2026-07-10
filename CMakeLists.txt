cmake_minimum_required(VERSION 3.20)
project(zlibpp VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找 zlib
find_package(ZLIB REQUIRED)

# 添加库
add_library(zlibpp INTERFACE)

# 设置包含目录
target_include_directories(zlibpp INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# 链接 zlib
target_link_libraries(zlibpp INTERFACE ZLIB::ZLIB)

# 设置编译特性
target_compile_features(zlibpp INTERFACE cxx_std_20)

# 安装
install(
        TARGETS zlibpp
        EXPORT zlibpp-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

# 安装头文件
install(
        DIRECTORY include/zlibpp/
        DESTINATION include/zlibpp
        FILES_MATCHING PATTERN "*.h"
)

# 安装 CMake 配置文件
install(
        EXPORT zlibpp-targets
        FILE zlibpp-targets.cmake
        NAMESPACE zlibpp::
        DESTINATION lib/cmake/zlibpp
)

# 生成配置文件
include(CMakePackageConfigHelpers)
configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/zlibpp-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/zlibpp-config.cmake
        INSTALL_DESTINATION lib/cmake/zlibpp
)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/zlibpp-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/zlibpp-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/zlibpp-config-version.cmake
        DESTINATION lib/cmake/zlibpp
)

# CPack 配置
set(CPACK_PACKAGE_NAME "zlibpp")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern C++ wrapper for zlib")
set(CPACK_PACKAGE_VENDOR "Your Name")
set(CPACK_PACKAGE_CONTACT "your.email@example.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# 生成源码包
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
        "/build/"
        "/.git/"
        "/.github/"
        "*.swp"
        "*.bak"
)

include(CPack)