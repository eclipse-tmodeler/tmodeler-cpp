# Copyright (c) 2026 TAMBO ai and others
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0

cmake_minimum_required(VERSION 3.24)

project(TModelerQtAdapter LANGUAGES CXX)

message(STATUS "🔧 Configuring Qt Adapter (TModelerQtAdapter)")

# Crée une bibliothèque INTERFACE (pas de compilation, juste des propriétés)
add_library(TModelerQtAdapter INTERFACE)

# Définir le chemin des headers (assume: adapters/Qt/)
set(TM_QT_ADAPTER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Inclure les headers publiquement (visible pour tous ceux qui linkent)
target_include_directories(TModelerQtAdapter INTERFACE
    ${TM_QT_ADAPTER_INCLUDE_DIR}
)

# Link à la lib TModeler
target_link_libraries(TModelerQtAdapter INTERFACE TModeler)