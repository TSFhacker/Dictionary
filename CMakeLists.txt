cmake_minimum_required(VERSION 2.6)
add_library(sqlite3 STATIC sqlite/sqlite3.c)

include_directories(sqlite)
add_executable(insert insert.c)
add_executable(select select.c)
foreach (example insert select)
  target_link_libraries(${example} sqlite3 pthread dl)
endforeach(example)


find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

add_definitions(${GTK3_CFLAGS_OTHER} -rdynamic)

target_link_libraries(select ${GTK3_LIBRARIES})