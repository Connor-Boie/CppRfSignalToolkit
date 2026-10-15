find_package(GTest CONFIG QUIET)

if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG b514bdc898e2951020cbdca1304b75f5950d1f59
    )
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(
    cpp_rf_signal_toolkit_tests
    iq_sample_test.cpp
    main_test.cpp
)

target_link_libraries(
    cpp_rf_signal_toolkit_tests
    PRIVATE
        CppRfSignalToolkit::core
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(cpp_rf_signal_toolkit_tests)
