cmake_minimum_required(VERSION 3.13.4)
project(llvm-verify-pass)

#===============================================================================
# 1. LOAD Z3 CONFIGURATION
#===============================================================================
find_package(Z3 REQUIRED CONFIG)
message(STATUS
  "Found Z3 ${Z3_VERSION_MAJOR}.${Z3_VERSION_MINOR}.${Z3_VERSION_PATCH}"
)
message(STATUS "Using Z3.cmake")

message("Z3Prover STATUS:
  Includes    ${Z3_CXX_INCLUDE_DIRS}
  Libraries   ${Z3_LIBRARIES}"
)

include_directories(SYSTEM ${Z3_CXX_INCLUDE_DIRS})
link_libraries(${Z3_LIBRARIES})

#===============================================================================
# 2. LOAD LLVM CONFIGURATION
#    For more: http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
#===============================================================================
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LT_LLVM_INSTALL_DIR}")

message("LLVM STATUS:
  Definitions ${LLVM_DEFINITIONS}
  Includes    ${LLVM_INCLUDE_DIRS}
  Libraries   ${LLVM_LIBRARY_DIRS}
  Targets     ${LLVM_TARGETS_TO_BUILD}"
)

# Set the LLVM header and library paths
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

#===============================================================================
# 2. LOAD BOOST CONFIGURATION
#===============================================================================
find_package(Boost 1.71 COMPONENTS program_options REQUIRED)
message(STATUS "Found BOOST ${BOOST_PACKAGE_VERSION}")
#message(STATUS "Using LLVMConfig.cmake in: ${LT_LLVM_INSTALL_DIR}")

message("BOOST STATUS:
  Includes    ${Boost_INCLUDE_DIR}
  Libraries   ${Boost_LIBRARIES}"
)

# Set the header and library paths
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARIES})
#add_definitions(${LLVM_DEFINITIONS})


#===============================================================================
# 3. LLVM-VERIFY-PASS BUILD CONFIGURATION
#===============================================================================
# Use the same C++ standard as LLVM does
set(CMAKE_CXX_STANDARD 14 CACHE STRING "")

# Build type
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE
      STRING "Build type (default Debug):" FORCE)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -fdiagnostics-color=always")

# LLVM is normally built without RTTI. Be consistent with that.
# if(NOT LLVM_ENABLE_RTTI)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
# endif()

# -fvisibility-inlines-hidden is set when building LLVM and on Darwin warnings
# are triggered if llvm-verify-pass is built without this flag
# (though otherwise it builds fine). For consistency, add it here too.
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(
  "-fvisibility-inlines-hidden" SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG
)
if (${SUPPORTS_FVISIBILITY_INLINES_HIDDEN_FLAG} EQUAL "1")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")

# Set the build directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

#===============================================================================
# 3. OTHER LIBRARIES REQUIRED
#===============================================================================
# Augeas Library and Libfaudes Library

include_directories(SYSTEM
                 #  "auges-latest/src/"
		   "augeas-1.4.0/src/"
                   "libfaudes-2_29d/include/"
                 #  "libfaudes-latest/libfaudes-2_29d/include/"
                   )

link_directories(
	      #	 "auges-latest/src/"
               "augeas-1.4.0/src/.libs/"
                "libfaudes-2_29d/"
              #  "libfaudes-latest/libfaudes-2_29d/"
                )

link_libraries(
              fa
              faudes
              )

#===============================================================================
# 4. ADD SUB-TARGETS
# Doing this at the end so that all definitions and link/include paths are
# available for the sub-projects.
#===============================================================================
add_subdirectory(lib)
