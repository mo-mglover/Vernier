/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2026 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>
#ifdef _OPENMP
  #include <omp.h>
#endif

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <string>

#include "mpi_context.h"
#include "affinity.h"

// Forward declarations
void check_characters(std::string const&, char, int const&);
int  get_max_threads();
int  get_thread_num();

//-------------------------------------------------------------------------------
// System call mocking
//-------------------------------------------------------------------------------

class MockOneThreadPerCore : public meto::AffinitySysCalls {
  public:
    int max_available_cpus() const override { return 64;}
    int num_available_cpus() const override { return 1;}
    int running_on_core()    const override { return get_thread_num();}
};

class MockTwoThreadsAlternateCores : public meto::AffinitySysCalls {
  public:
    int max_available_cpus() const override { return 64;}
    int num_available_cpus() const override { return 1;}
    int running_on_core()    const override { return (get_thread_num()/2)*2;}
};

//-------------------------------------------------------------------------------
// One thread per core
//-------------------------------------------------------------------------------

TEST(AffinityTest, OneThreadPerCore) {

  // Need a valid Vernier MPI context
  auto mpi_context = meto::MPIContext();
  mpi_context.init(MPI_COMM_WORLD, "comm_world");

  auto mock = std::make_unique<MockOneThreadPerCore>();
  int const max_available_cpus = mock->max_available_cpus();
  meto::Affinity affinity(std::move(mock));

  std::string fname = "vernier-affinity-one-thread-per-core.txt";
  affinity.write_map(mpi_context, fname);

  int constexpr expected_hashes_per_line = 0;
  check_characters(fname, '#', expected_hashes_per_line);
  check_characters(fname, '.', max_available_cpus - get_max_threads());

  mpi_context.finalize();

}

//-------------------------------------------------------------------------------
// Main test
//-------------------------------------------------------------------------------

TEST(AffinityTest, TwoThreadsAlternateCores) {

  // Need a valid Vernier MPI context
  auto mpi_context = meto::MPIContext();
  mpi_context.init(MPI_COMM_WORLD, "comm_world");

  auto mock = std::make_unique<MockTwoThreadsAlternateCores>();
  int const max_available_cpus = mock->max_available_cpus();
  meto::Affinity affinity(std::move(mock));

  std::string fname = "vernier-affinity-two-threads-alternate-cores.txt";
  affinity.write_map(mpi_context, fname);

  int const expected_hashes_per_line = (get_max_threads()+1)/2;
  check_characters(fname, '#', expected_hashes_per_line);
  check_characters(fname, '.', max_available_cpus - expected_hashes_per_line);

  mpi_context.finalize();

}

//-------------------------------------------------------------------------------
// Main test
//-------------------------------------------------------------------------------

TEST(AffinityTest, TestSequence) {

  // Need a valid Vernier MPI context
  auto mpi_context = meto::MPIContext();
  mpi_context.init(MPI_COMM_WORLD, "comm_world");

  auto mock = std::make_unique<MockOneThreadPerCore>();
  int const max_available_cpus = mock->max_available_cpus();
  meto::Affinity affinity(std::move(mock));

  // Expected sequence.
  std::string const
   sequence = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ~~";

  // Generate the output sequence.
  int index = 0;
  std::string str(static_cast<std::size_t>(max_available_cpus), '.');
  std::generate(str.begin(), str.end(), [&index, &affinity]() {
    return affinity.hex(index++);
  });

  EXPECT_EQ(str, sequence) 
  << "Generated string does not match expected sequence.";

}

//-------------------------------------------------------------------------------
// Check characters
//-------------------------------------------------------------------------------

void check_characters(std::string const& fname, char digit, int const& expected_per_line) {

  std::ifstream file (fname);
  EXPECT_TRUE(file.is_open());

  std::string line_in_file;
  int line_number = 0;

  // Read past the header
  while(std::getline(file, line_in_file)) {
    if (line_in_file.find("Cores ---->") != std::string::npos) {
      break;
    }
  }

  while(std::getline(file, line_in_file)) {
    ++line_number;

    auto num_matches_in_line = std::count(
      line_in_file.begin(),
      line_in_file.end(),
      digit
    );

    EXPECT_EQ(num_matches_in_line, expected_per_line)
      << "Incorrect number of matches of " << digit 
      << " on line: " << line_number << ":\n"
      << line_in_file;

  }
}

//-------------------------------------------------------------------------------
// Get the number of threads
//-------------------------------------------------------------------------------

int get_max_threads() {

  #ifdef _OPENMP
    return omp_get_max_threads();
  #else
    return 1;
  #endif

}

//-------------------------------------------------------------------------------
// Get the thread ID
//-------------------------------------------------------------------------------

int get_thread_num() {

  #ifdef _OPENMP
    return omp_get_thread_num();
  #else
    return 0;
  #endif

}

