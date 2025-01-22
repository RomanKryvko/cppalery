cmake_minimum_required(VERSION 3.25)

project(cppalery VERSION 1.2.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_DATE)
string(TIMESTAMP BUILD_DATE "%Y %B")

add_executable(cppalery)

set(CPPALERY_SOURCES
    "src/directoryPager.cpp"
    "src/keybindings.cpp"
    "src/pager.cpp"
    "src/directorySearcher.cpp"
    "src/directoryController.cpp"
    "src/backgroundSetter.cpp"
    "src/commandWindow.cpp"
    "src/directory.cpp"
    "src/form.cpp"
    "src/main.cpp"
    "src/mainWindow.cpp"
    "src/previewWindow.cpp"
    "src/window.cpp"
    "src/config.cpp"
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(IMLIB2 REQUIRED IMPORTED_TARGET GLOBAL imlib2)

set(CPPALERY_LIBRARIES
    X11
    ncurses
    PkgConfig::IMLIB2
)

configure_file(include/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h)
configure_file(docs/cppalery.1.in cppalery.1)

target_sources(cppalery PRIVATE "${CPPALERY_SOURCES}")
target_include_directories(cppalery PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(cppalery PRIVATE "${CPPALERY_LIBRARIES}")

install(TARGETS cppalery RUNTIME)
install(FILES "${PROJECT_BINARY_DIR}/cppalery" TYPE BIN
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                    GROUP_READ GROUP_EXECUTE
                    WORLD_READ WORLD_EXECUTE)
install(FILES "${PROJECT_BINARY_DIR}/cppalery.1"
        DESTINATION "${CMAKE_INSTALL_MANDIR}/man1")
