set(CMAKE_C_CLANG_TIDY "/usr/bin/clang-tidy" "--header-filter=$ENV{CI_PROJECT_DIR}/([A-SV-Z]|Testing)" CACHE STRING "")
set(CMAKE_CXX_CLANG_TIDY "/usr/bin/clang-tidy" "--header-filter=$ENV{CI_PROJECT_DIR}/([A-SV-Z]|Testing)" CACHE STRING "")

include("${CMAKE_CURRENT_LIST_DIR}/configure_fedora34.cmake")
