cmake_minimum_required (VERSION 3.8)

project ("RunAsTI")

add_definitions(-DUNICODE -D_UNICODE)
add_executable (RunAsTI "RunAsTI.cpp" "Utils.h" "Utils.cpp" "RunAsTI.rc")
set_target_properties(RunAsTI PROPERTIES LINK_FLAGS " /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RunAsTI PROPERTY CXX_STANDARD 20)
endif()
