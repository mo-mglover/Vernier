/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2026 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "vernier.h"
#include "vernier_scope.h"
#include "vernier_get_wtime.h"

// -------------------------------------------------------------------------------
//  Supporting function prototypes
// -------------------------------------------------------------------------------

void sub_one();
void sub_two();

// -------------------------------------------------------------------------------
//  Main tests
// -------------------------------------------------------------------------------

TEST(ScopeTest, ScopeTest) {

  meto::vernier.init();

  // We will need the scope handle outside the scoped region.
  size_t scope_handle;

  double t1 = meto::vernier_get_wtime();
  {
    auto vnr_scope = meto::VernierScope("main_scope");

    // Store the scope handle.
    scope_handle = vnr_scope.get_handle();

    // Call a region
    sub_one();

    // Call nested region on many threads.
    #pragma omp parallel
    {
      sub_two();
    }
  }
  double t2 = meto::vernier_get_wtime();

  {
    double const time_tolerance = 0.0005;
    double actual_time = t2 - t1;
    double scope_time = meto::vernier.get_total_walltime(scope_handle, 0);
    EXPECT_NEAR(scope_time, actual_time, time_tolerance);
  }

  meto::vernier.finalize();
}

// -------------------------------------------------------------------------------
//  Supporting functions
// -------------------------------------------------------------------------------

void sub_one(){
   auto vnr_scope = meto::VernierScope("sub_one");
   sleep(1);
}

void sub_two(){
  auto vnr_scope = meto::VernierScope("sub_two");
  sub_one();
  sleep(1);
}
