# ----------------------------------------------------------------------------
# (c) Crown copyright 2024 Met Office. All rights reserved.
# The file LICENCE, distributed with this code, contains details of the terms
# under which the code may be used.
# ----------------------------------------------------------------------------

# Tests including call depth functionality.
set_tests_properties(RegionNameTest.NamesMatchTestDepth
                     PROPERTIES ENVIRONMENT "VERNIER_ADD_CALL_DEPTH=true")

set_tests_properties(HashTableTest.HashFunctionTestDepth
                     PROPERTIES ENVIRONMENT "VERNIER_ADD_CALL_DEPTH=true")

set_tests_properties(HashTableTest.UpdateTimesTestDepth
                     PROPERTIES ENVIRONMENT "VERNIER_ADD_CALL_DEPTH=true")

