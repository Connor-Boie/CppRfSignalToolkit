add_executable(cpp_rf_signal_toolkit main.cpp)

target_compile_options(
    cpp_rf_signal_toolkit
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
)
