# Program Library
# SafetyVerificationPass Library
add_library(SafetyVerificationPass SHARED
		SafetyVerificationPass.cpp
		Program.cpp
		AFA/AFAut.cpp
		AFA/AFAState.cpp
		AFA/PassTwo.cpp
		AFA/PassThree.cpp
		AFA/PassFourWeak.cpp
		AFA/PassFourAFA.cpp
		AFA/DotToAFA.cpp
		SCTransSystem.cpp
		Utils.cpp
		)

target_include_directories(SafetyVerificationPass PUBLIC
		"../include/"
		)

target_link_libraries(SafetyVerificationPass
		boost_chrono
		faudes
		)

set(SOURCES AFA/AFAut.cpp
#		SafetyVerificationPass.cpp
#		Program.cpp
		AFA/AFAState.cpp
		AFA/PassTwo.cpp
		AFA/PassThree.cpp
		AFA/PassFourWeak.cpp
		AFA/PassFourAFA.cpp
#		SCTransSystem.cpp
		Utils.cpp
		OnTheFly.cpp
		MetaState.cpp
		)

add_executable("OnTheFly" ${SOURCES})

target_include_directories("OnTheFly" PUBLIC
		"../include/"
		)

#find_package(Boost 1.65 COMPONENTS program_options REQUIRED)
#include_directories("DotToAFA" ${Boost_INCLUDE_DIR})
#link_directories("DotToAFA" ${Boost_LIBRARIES})

target_link_libraries("OnTheFly"
		boost_chrono
		boost_graph
		faudes
		LLVMSupport

		)

message("successful ")


#set(SOURCES AFA/DotToAFA.cpp SafetyVerificationPass.cpp
#		Program.cpp
#		AFA/AFAut.cpp
#		AFA/AFAState.cpp
#		AFA/PassTwo.cpp
#		AFA/PassThree.cpp
#		AFA/PassFourWeak.cpp
#		AFA/PassFourAFA.cpp
#		SCTransSystem.cpp
#		Utils.cpp
#		)

set(SOURCES AFA/DotToAFA.cpp
		AFA/AFAut.cpp
		AFA/AFAState.cpp
		AFA/PassTwo.cpp
		AFA/PassThree.cpp
		AFA/PassFourWeak.cpp
		AFA/PassFourAFA.cpp
		Utils.cpp
		)

add_executable("DotToAFA" ${SOURCES})

#add_executable("DotToAFA" "AFA/DotToAFA.cpp")

target_include_directories("DotToAFA" PUBLIC
		"../include/"
		)

#find_package(Boost 1.65 COMPONENTS program_options REQUIRED)
#include_directories("DotToAFA" ${Boost_INCLUDE_DIR})
#link_directories("DotToAFA" ${Boost_LIBRARIES})

target_link_libraries("DotToAFA"
		#	boost_chrono
		boost_graph
		faudes
		LLVMSupport
		m
		dl
		)

set(GCC_COVERAGE_COMPILE_FLAGS "-pedantic -g -lboost_graph -lboost_system")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

message("Yup, works!!")
#add_library(Program STATIC Program.cpp)

#target_include_directories(Program PUBLIC
#                          "../include/"
#                          )


#link_libraries(Program)

# AFA Library
#add_library(AFAut STATIC AFA/AFAut.cpp)


#target_include_directories(AFAut PUBLIC
#                          "../include/"
#                          )
# AFAState Library
#add_library(AFAState STATIC AFA/AFAState.cpp)


#target_include_directories(AFAState PUBLIC
#                         "../include/"
#                        )

# AFA Pass Two Library
#add_library(PassTwo STATIC AFA/PassTwo.cpp)


#target_include_directories(PassTwo PUBLIC
#                          "../include/"
#                          )

# AFA Pass Three Library
#add_library(PassThree STATIC AFA/PassThree.cpp)


#target_include_directories(PassThree PUBLIC
#                          "../include/"
#                          )

# AFA Pass Four Weak- Library
#add_library(PassFourWeak STATIC AFA/PassFourWeak.cpp)

#target_include_directories(PassFourWeak PUBLIC
#                          "../include/"
#                          )

# AFA Pass Four AFA- Library
#add_library(PassFourAFA STATIC AFA/PassFourAFA.cpp)


#target_include_directories(PassFourAFA PUBLIC
#                          "../include/"
#                          )


# SCTransSystem Library
#add_library(SCTransSystem STATIC SCTransSystem.cpp)

#target_include_directories(SCTransSystem PUBLIC
#                          "../include/"
#                          )

#target_link_libraries(SCTransSystem
#                     Program
#		     fa
#                     )

