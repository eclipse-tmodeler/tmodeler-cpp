cmake_minimum_required(VERSION 3.24)

# Indiquer que ce projet utilise le langage C
project(SQLite3 LANGUAGES C)

# Création d'une lib statique pour SQLite
add_library(SQLite3 STATIC sqlite3.c)

# ✅ Définir la macro uniquement pour sqlite3.c
set_source_files_properties(
    sqlite3.c
    PROPERTIES COMPILE_DEFINITIONS "__STDC_VERSION__=201710L"
)

# ✅ Ajouter toutes les macros de compilation nécessaires
target_compile_definitions(SQLite3 PRIVATE
    SQLITE_THREADSAFE=1
    SQLITE_ENABLE_FTS5
    SQLITE_ENABLE_COLUMN_METADATA
    SQLITE_ENABLE_RTREE
)

# Inclure le répertoire actuel en privé (interne à la lib)
target_include_directories(SQLite3
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

install(TARGETS SQLite3
  EXPORT SQLite3Targets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(EXPORT SQLite3Targets
  FILE SQLite3Targets.cmake
  NAMESPACE SQLite::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SQLite3
)
