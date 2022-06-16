# Set build-directive (used in core to tell which buildtype we used)
add_definitions(-D_BUILD_DIRECTIVE="${CONFIGURATION}")

if(WITH_WARNINGS)
  set(WARNING_FLAGS "-Wall -Werror -Winit-self -Wno-mismatched-tags -Werror=return-type")
  set(NO_WARNING_FLAGS "-Wno-reorder -Wno-unknown-pragmas -Wno-unused-function -Wno-unused-private-field -Wno-unused-value -Wno-unused-variable -Wno-unused-const-variable -Wno-undefined-var-template -Wno-unused-lambda-capture")

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS} ${NO_WARNING_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} -Woverloaded-virtual ${NO_WARNING_FLAGS}")
  message(STATUS "Clang: All warnings enabled")
else()
  add_definitions(--no-warnings)
  message(STATUS "Clang: All warnings disabled")
endif()

if(WITH_COREDEBUG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
  message(STATUS "Clang: Debug-flags set (-g3)")
endif()

# -Wno-narrowing needed to suppress a warning in g3d
# -Wno-deprecated-register is needed to suppress 185 gsoap warnings on Unix systems.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-narrowing -Wno-deprecated-register -Wno-shorten-64-to-32 -Wno-delete-non-virtual-dtor")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1")
