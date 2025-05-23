/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2022 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 *  @file   hashtable.h
 *  @brief  Handles entries for each timed region.
 *
 *  In order to store region timings, one struct and one class are defined. The
 *  struct (RegionRecord) collects together information pertinent to a single
 *  profiled region, such as its name, total time and self time.
 *
 *  The HashTable class contains a hashtable to hold the hash entries (see
 *  above). The hashing algorithm is bundled with it, so that it remains an
 *  internal implementation detail only.
 *
 *  An assortment of methods handle admin. tasks, such as adding new entries.
 *
 */

#ifndef VERNIER_HASHTABLE_H
#define VERNIER_HASHTABLE_H

#include <unordered_map>

#include "hashvec.h"
#include "vernier_gettime.h"

#define PROF_HASHVEC_RESERVE_SIZE 1000
#ifndef PROF_STRING_BUFFER_LENGTH
#define PROF_STRING_BUFFER_LENGTH 100
#endif

namespace meto {

// Forward declarations
class HashVecHandler;

/**
 * @brief  Defines a null hash function.
 *
 * Having already hashed region names, we won't need to hash the hashtable keys.
 *
 */

struct NullHashFunction {
  std::size_t operator()(std::size_t const &key) const { return key; }
};

/**
 * @brief  Wraps STL hashtables with additional functionality.
 *
 * Bundles together an STL hashtable with the hashing algorithm, and adds
 * e.g. time-handling methods.
 *
 */

class HashTable {

private:
  // Members
  int tid_;
  size_t profiler_hash_;
  record_index_t profiler_index_;

  // Hash function
  std::hash<std::string_view> hash_function_;

  // Hashtable containing locations of region records.
  std::unordered_map<size_t, record_index_t, NullHashFunction> lookup_table_;

  // Vector of region records.
  hashvec_t hashvec_;

  // Private member functions
  void prepare_computed_times(RegionRecord &);
  void prepare_computed_times_all();
  void sort_records();
  void erase_record(size_t const);
  void sync_lookup();
  RegionRecord &hash2record(size_t const);
  RegionRecord const &hash2record(size_t const) const;

public:
  // Constructors
  HashTable() = delete;
  HashTable(int);

  // Prototypes
  size_t compute_hash(std::string_view, int);
  void query_insert(std::string_view const, int, size_t &,
                    record_index_t &) noexcept;
  void update(record_index_t const, time_duration_t const);

  // Member functions
  std::vector<size_t> list_keys();

  void add_child_time_to_parent(record_index_t const, time_duration_t const,
                                time_duration_t *&);
  void add_profiler_call(time_duration_t *&);

  void compute_self_times();
  void append_to(HashVecHandler &);

  // Getters
  double get_total_walltime(size_t const hash) const;
  double get_overhead_walltime(size_t const hash) const;
  double get_self_walltime(size_t const hash);
  double get_child_walltime(size_t const hash) const;
  std::string get_decorated_region_name(size_t const hash) const;
  unsigned long long int get_call_count(size_t const hash) const;
  unsigned long long int get_prof_call_count() const;

  void increment_recursion_level(record_index_t const);
  void decrement_recursion_level(record_index_t const);
};

} // namespace meto

#endif
