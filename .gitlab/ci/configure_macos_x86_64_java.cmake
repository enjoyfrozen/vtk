include("${CMAKE_CURRENT_LIST_DIR}/configure_macos.cmake")

# Ensure that we're targeting 11.0.
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "")
