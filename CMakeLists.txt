cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 17)

project("StudentMarkMgrSystem"
    LANGUAGES C
    VERSION 0.1
)

add_executable(${CMAKE_PROJECT_NAME})
enable_language(C)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    "src/main.c"
    "src/util/seq.c"
    "src/util/memory.c"
    "src/util/stringUtil.c"
    "src/util/data.c"
    "src/io/tablePrinter.c"
    "src/io/csvReader.c"
    "src/io/csvWriter.c"
    "src/struct/table.c"
    "src/struct/subTable.c"
    "src/struct/item.c"
    "src/ui/input.c"
    "src/ui/menu.c"
    "src/ui/mainMenu.c"
    "src/ui/findMenu.c"
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    "./src"
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINK_SEARCH_END_STATIC 1)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc")