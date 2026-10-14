#include <iostream>
#include <string_view>

namespace {

constexpr std::string_view application_name{"CppRfSignalToolkit"};

}  // namespace

int main()
{
    std::cout << application_name << '\n';
    return 0;
}
