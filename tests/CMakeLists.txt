find_package(Catch2 CONFIG REQUIRED)

enable_testing()

add_executable(unit_tests
        Scanner_TestCase.cpp)
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain goo_lib)

include(CTest)
include(Catch)
catch_discover_tests(unit_tests)