add_library(cpp_rf_signal_toolkit_core STATIC raw_iq_reader.cpp)
add_library(CppRfSignalToolkit::core ALIAS cpp_rf_signal_toolkit_core)

target_include_directories(
    cpp_rf_signal_toolkit_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(cpp_rf_signal_toolkit_core PUBLIC cxx_std_20)

target_compile_options(
    cpp_rf_signal_toolkit_core
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
)

add_executable(cpp_rf_signal_toolkit main.cpp)
target_link_libraries(cpp_rf_signal_toolkit PRIVATE CppRfSignalToolkit::core)

target_compile_options(
    cpp_rf_signal_toolkit
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
)
