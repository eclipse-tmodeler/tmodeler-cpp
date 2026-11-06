# Copyright (c) 2026 TAMBO ai and others
#
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0

cmake_minimum_required(VERSION 3.16)

# 📥 Trouver wxWidgets via son package config
find_package(wxWidgets 3.2 COMPONENTS core base REQUIRED CONFIG)

if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()

# 📁 Fichiers pour runTests
set(TESTS_TM_SRC
    modelsTM.cpp
    modelsTM.hpp
	
    TestsTM.cpp
)

# 📁 Fichiers pour runViewer
set(VIEWER_SRC
    modelsTM.cpp
    modelsTM.hpp
	
	ViewerApp.cpp
    MyCanvas.cpp
    MyCanvas.h
)

# ✅ Exécutable runTests avec GoogleTest
add_executable(runTests ${TESTS_TM_SRC})
target_link_libraries(runTests
    PRIVATE
        TModeler
        GTest::gtest
        GTest::gtest_main
)
add_test(NAME RunAllTests COMMAND runTests)

# 🎯 Exécutable runViewer avec wxWidgets
add_executable(runViewer WIN32 ${VIEWER_SRC})

target_link_libraries(runViewer PRIVATE TModeler ${wxWidgets_LIBRARIES})

set_target_properties(runViewer PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
