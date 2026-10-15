add_library(cpp_rf_signal_toolkit_core INTERFACE)
add_library(CppRfSignalToolkit::core ALIAS cpp_rf_signal_toolkit_core)

target_include_directories(
    cpp_rf_signal_toolkit_core
    INTERFACE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(cpp_rf_signal_toolkit_core INTERFACE cxx_std_20)

add_executable(cpp_rf_signal_toolkit main.cpp)
target_link_libraries(cpp_rf_signal_toolkit PRIVATE CppRfSignalToolkit::core)

target_compile_options(
    cpp_rf_signal_toolkit
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
)
