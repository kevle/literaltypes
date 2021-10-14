add_library(WarningConfiguration INTERFACE)
if(MSVC)
    target_compile_options(WarningConfiguration INTERFACE /W4)
else(MSVC)
    target_compile_options(WarningConfiguration INTERFACE -Wall -Wextra -pedantic)
endif(MSVC)

add_executable(first "first.cpp")
target_link_libraries(first PUBLIC literaltypes WarningConfiguration)