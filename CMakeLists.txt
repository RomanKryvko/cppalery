cmake_minimum_required(VERSION 3.25)

project(Cppalery)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
add_executable(cppalery)

set(CPPALERY_SOURCES)
set(CPPALERY_LIBRARIES)

list(
    APPEND
    CPPALERY_SOURCES
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

list(
    APPEND
    CPPALERY_LIBRARIES
    ncurses
)

target_sources(cppalery PRIVATE "${CPPALERY_SOURCES}")
target_include_directories(cppalery PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(cppalery PRIVATE "${CPPALERY_LIBRARIES}")
