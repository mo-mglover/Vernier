/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2023 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 *  @file   hashvec.h
 *  @brief  Defines struct and datatype for timed region entries in the
 *          hash vector.
 */

#ifndef VERNIER_HASHVEC_H
#define VERNIER_HASHVEC_H

#include <string>
#include <string_view>
#include <vector>

#include "vernier_gettime.h"

// Define the call depth type.
using call_depth_t = int;
call_depth_t constexpr null_call_depth = -1;

namespace meto {

/**
 * @brief  Structure to hold information for a particular region.
 *
 * Bundles together any information pertinent to a specific profiled region.
 *
 */

struct RegionRecord {
public:
  // Constructor
  RegionRecord() = delete;
  explicit RegionRecord(size_t const, std::string_view const, int const,
                        call_depth_t const, std::string_view parent_region_name);

  // Data members
  size_t region_hash_;
  std::string region_name_;
  std::string decorated_region_name_;
  call_depth_t    call_depth_;
  time_duration_t total_walltime_;
  time_duration_t recursion_total_walltime_;
  time_duration_t self_walltime_;
  time_duration_t child_walltime_;
  time_duration_t overhead_walltime_;
  unsigned long long int call_count_;
  unsigned int recursion_level_;
};

// Define the hashvec type.
using hashvec_t = std::vector<RegionRecord>;

// Type definitions
using record_index_t = std::vector<RegionRecord>::size_type;

} // namespace meto

#endif
