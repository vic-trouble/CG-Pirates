cmake_minimum_required(VERSION 3.0)

project(Pirates)

if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(BUILD_TESTS "Build tests (depends on the Google Test)" OFF)

if (${CMAKE_GENERATOR} MATCHES "Visual Studio.*201[357]")
    add_definitions(-DHAS_MAKEUNIQUE)
endif()

add_library(Pirates.Lib
    commands.h commands.cpp
    cxxtweaks.h
    game.h game.cpp
    hex.h hex.cpp
    io.h io.cpp
    log.h
    simulation.h simulation.cpp
    )

add_library(Pirates.AI
    ai_naive.h ai_naive.cpp
    ai_score.h ai_score.cpp
    ai_search.h ai_search.cpp
    ai_tactics.h ai_tactics.cpp
    )

add_executable(Pirates.App
    main.cpp
    )
target_link_libraries(Pirates.App PRIVATE Pirates.Lib Pirates.AI)

if(BUILD_TESTS)
    enable_testing()

    include(ExternalProject)
    set(GTEST_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install)
    set(GTEST_INCLUDE_DIR ${GTEST_INSTALL_DIR}/include)
    set(GTEST_LIB_DIR ${GTEST_INSTALL_DIR}/lib)
    set(GTEST_LIB ${GTEST_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX})

    ExternalProject_Add(gtest
        GIT_REPOSITORY https://github.com/google/googletest GIT_TAG "release-1.8.0"
        UPDATE_COMMAND ""
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${GTEST_INSTALL_DIR} -Dgtest_force_shared_crt=ON)

    add_executable(Pirates.Test
        main_test.cpp
        test_hex.cpp
        test_simulation.cpp
        )
    add_dependencies(Pirates.Test gtest)
    target_link_libraries(Pirates.Test PRIVATE Pirates.Lib ${GTEST_LIB})
    target_include_directories(Pirates.Test PRIVATE ${GTEST_INCLUDE_DIR})

    add_test(NAME Pirates.Tests COMMAND $<TARGET_FILE:Pirates.Test>)

    add_executable(Pirates.AI.Test
        assertions.h
        main_test.cpp
        proxies.h
        test_ai_naive.cpp
        test_ai_score.cpp
        test_ai_search.cpp
        test_ai_tactics.cpp
        )
    add_dependencies(Pirates.AI.Test gtest)
    target_link_libraries(Pirates.AI.Test PRIVATE Pirates.AI Pirates.Lib ${GTEST_LIB})
    target_include_directories(Pirates.AI.Test PRIVATE ${GTEST_INCLUDE_DIR})

    add_test(NAME Pirates.AI.Tests COMMAND $<TARGET_FILE:Pirates.AI.Test>)
endif()
