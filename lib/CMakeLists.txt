# Program Library
add_library(Program SHARED Program.cpp)

target_include_directories(Program PUBLIC
                          "../include/"
                          )

link_libraries(Program)

# AFA Library
add_library(AFAut SHARED AFA/AFAut.cpp)


target_include_directories(AFAut PUBLIC
                          "../include/"
                          )

# SCTransSystem Library
add_library(SCTransSystem SHARED SCTransSystem.cpp)

target_include_directories(SCTransSystem PUBLIC
                          "../include/"
                          )

target_link_libraries(SCTransSystem
                     Program
                     )

# SafetyVerificationPass Library
add_library(SafetyVerificationPass SHARED SafetyVerificationPass.cpp)

target_include_directories(SafetyVerificationPass PUBLIC
                          "../include/"
                          )

 target_link_libraries(SafetyVerificationPass
                      Program
                      )
