# Program Library
add_library(Program SHARED Program.cpp)

target_include_directories(Program PUBLIC
                          "../include/"
                          )

# SafetyVerificationPass Library
add_library(SafetyVerificationPass SHARED SafetyVerificationPass.cpp)

target_include_directories(SafetyVerificationPass PUBLIC
                          "../include/"
                          )

target_link_libraries(SafetyVerificationPass
                     Program
                     )
