cmake_minimum_required(VERSION 3.16) # минимальная нужная версия для cmake
project(cache-cleaner VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20) # версия c++
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # compile_commands.json для clang

# импорт библиотек
find_package(PkgConfig REQUIRED)
find_package(nlohmann_json REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)

set(SOURCES
    src/main.cpp

    src/core/cleaner.cpp
    src/core/config_loader.cpp
    src/core/constants.cpp
    src/core/cli_tools.cpp

    src/ui/main_window.cpp

    src/utils/console_logger.cpp
    src/utils/args_parser.cpp
    src/utils/system_detector.cpp
)

# компиляция всех файлов
add_executable(${PROJECT_NAME} ${SOURCES})

# чтоб писать modules/... вместо src/modules/...
target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PRIVATE
    PkgConfig::GTKMM
    nlohmann_json::nlohmann_json
)

# компиляция библиотек ОДИН РАЗ
target_precompile_headers(${PROJECT_NAME} PRIVATE
    <gtkmm.h>
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    PROJECT_NAME="${PROJECT_NAME}"
    PROJECT_VERSION="${PROJECT_VERSION}"
)
