# ------------------------------------------------------------------------------
#  (c) Crown copyright 2022 Met Office. All rights reserved.
#  The file LICENCE, distributed with this code, contains details of the terms
#  under which the code may be used.
# ------------------------------------------------------------------------------
set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/modules)

# Testing for building a fortran lib and executable.
add_library(${CMAKE_PROJECT_NAME}_f SHARED "vernier_mod.F90")

set(PLIBS OpenMP::OpenMP_Fortran)
if (ENABLE_MPI)
  list(APPEND PLIBS MPI::MPI_Fortran)
endif ()

target_link_libraries(${CMAKE_PROJECT_NAME}_f PUBLIC
                      ${CMAKE_PROJECT_NAME}_c
                      ${PLIBS}
                      )

target_include_directories(${CMAKE_PROJECT_NAME}_f
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)

include(FortranCInterface)
FortranCInterface_VERIFY()
FortranCInterface_VERIFY(CXX)
FortranCInterface_HEADER(FC.h MACRO_NAMESPACE "FC_")

# Set the properties of the library including, the version and language. Also
# set which header files are to be public and installed, this can be useful
# when limiting access to the internal workings of the library with the install.
set_target_properties(${CMAKE_PROJECT_NAME}_f PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
        LINKER_LANGUAGE Fortran)

# Set up installation rules for the library.
install(TARGETS ${CMAKE_PROJECT_NAME}_f EXPORT "${CMAKE_PROJECT_NAME}Targets"
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT           "${CMAKE_PROJECT_NAME}_RunTime"
        NAMELINK_COMPONENT  "${CMAKE_PROJECT_NAME}_Development"
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# On installation, copy .mod files to the `include` directory.
install(DIRECTORY ${CMAKE_Fortran_MODULE_DIRECTORY}/ DESTINATION include)
