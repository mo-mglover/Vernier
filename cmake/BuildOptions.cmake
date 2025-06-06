# ------------------------------------------------------------------------------
#  (c) Crown copyright 2022 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
# This sets up the option to run without certain libraries if we want to test if
# Vernier runs without them.

# Optional argument to build with OpenMP.
option(BUILD_OPENMP "Build with OpenMP" ON)

set(OPENMP ${CMAKE_DL_LIBS})

if(BUILD_OPENMP)
    list(APPEND OPENMP OpenMP::OpenMP_CXX OpenMP::OpenMP_Fortran)
endif()

# Optional profiler string length
option(STRING_LENGTH "Maximum identifer string length" 100)

# Whether to create a vernier.pc pkgc-config file
option(ENABLE_PKGCONFIG "Enable pkg-config support" ON)
