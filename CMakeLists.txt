# Copyright (c) 2026 TAMBO ai and others
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0

cmake_minimum_required(VERSION 3.24)
project(TModeler LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Pour une meilleure compatibilité cross-plateforme
include(GNUInstallDirs)

# Activer les tests unitaires
enable_testing()

# 📥 Dépendances externes via FetchContent
include(FetchContent)

# Répertoire partagé pour stocker les dépendances téléchargées
if(MSVC)
    # Visual Studio / MSVC
    set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/external2)
elseif(MINGW)
    # MinGW (ex: QtCreator MinGW kits)
    set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/external4)
else()
    # GNU/Linux et fallback toolchains
    set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/external)
endif()

# JSON
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Fichiers source de la lib TModeler
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/core/*.cpp
    src/db/*.cpp
    src/modeler/*.cpp
    src/model/*.cpp
    #src/matrix/*.cpp
    src/field/*.cpp
    src/ms/*.cpp
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS 
    ${CMAKE_SOURCE_DIR}/include/core/*.h*
    ${CMAKE_SOURCE_DIR}/include/db/*.h*
    ${CMAKE_SOURCE_DIR}/include/modeler/*.h*
    ${CMAKE_SOURCE_DIR}/include/model/*.h*
    #${CMAKE_SOURCE_DIR}/include/matrix/*.h*
    ${CMAKE_SOURCE_DIR}/include/field/*.h*
    ${CMAKE_SOURCE_DIR}/include/ms/*.h*
)

if(NOT SOURCES)
    message(FATAL_ERROR "Aucun fichier source trouvé")
endif()

if(NOT HEADERS)
    message(FATAL_ERROR "Aucun header trouvé")
endif()

#message(STATUS "Sources: ${SOURCES}")
#message(STATUS "Headers: ${HEADERS}")

# Ajout des fichiers SQLite à la bibliothèque TModeler
list(APPEND SOURCES
    ${CMAKE_SOURCE_DIR}/dependencies/sqlite/sqlite3.c
)

# Création de la bibliothèque
add_library(TModeler ${SOURCES} ${HEADERS})

# Définitions spécifiques MinGW (notamment kits QtCreator MinGW)
if(MINGW)
    target_compile_definitions(TModeler PRIVATE
        _GLIBCXX_HAS_GTHREADS
        _GLIBCXX_USE_WEAK_REF=0
    )
endif()

# Lien vers la dépendance nlohmann/json
target_link_libraries(TModeler PUBLIC nlohmann_json::nlohmann_json)

# Lien vers SQLite sans exposer directement les fichiers SQLite
target_include_directories(TModeler
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/dependencies/sqlite>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/sqlite
)

# Désactiver les warnings SQLite en Release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(TModeler PRIVATE NDEBUG)
endif()

# Définitions spécifiques SQLite
target_compile_definitions(TModeler PRIVATE
    SQLITE_THREADSAFE=1
    SQLITE_ENABLE_FTS5
    SQLITE_ENABLE_COLUMN_METADATA
    SQLITE_ENABLE_RTREE
)

# Installation de la cible TModeler et de ses fichiers d'export
install(TARGETS TModeler nlohmann_json
    EXPORT TModelerTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT TModelerTargets
    FILE TModelerTargets.cmake
    NAMESPACE TModeler::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TModeler
)

# 📂 Organisation pour Visual Studio
source_group("Source Files\\Core" REGULAR_EXPRESSION ".*/core/.*\\.cpp")
source_group("Source Files\\db" REGULAR_EXPRESSION ".*/db/.*\\.cpp")
source_group("Source Files\\Modeler" REGULAR_EXPRESSION ".*/modeler/.*\\.cpp")
source_group("Source Files\\Model" REGULAR_EXPRESSION ".*/model/.*\\.cpp")
source_group("Source Files\\Field" REGULAR_EXPRESSION ".*/field/.*\\.cpp")
source_group("Source Files\\MS" REGULAR_EXPRESSION ".*/ms/.*\\.cpp")

source_group("Header Files\\Core" REGULAR_EXPRESSION ".*/include/core/.*\\.h")
source_group("Header Files\\db" REGULAR_EXPRESSION ".*/include/db/.*\\.h")
source_group("Header Files\\Modeler" REGULAR_EXPRESSION ".*/include/modeler/.*\\.h")
source_group("Header Files\\Model" REGULAR_EXPRESSION ".*/include/model/.*\\.h")
source_group("Header Files\\Field" REGULAR_EXPRESSION ".*/include/field/.*\\.h")
source_group("Header Files\\MS" REGULAR_EXPRESSION ".*/include/ms/.*\\.h")

# 🔬 Tests
add_subdirectory(tests)