/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2023 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include "hashvec.h"

/**
 * @brief  Constructs a new region record.
 * @param [in]  region_hash  Hash of the region name.
 * @param [in]  region_name  The region name.
 * @param [in]  tid          The thread id.
 */

meto::RegionRecord::RegionRecord(size_t const region_hash,
                                 std::string_view const region_name,
                                 int const tid, call_depth_t const call_depth,
                                 std::string_view const parent_region_name)
    : region_hash_(region_hash), region_name_(region_name),
      call_depth_(call_depth),
      total_walltime_(time_duration_t::zero()),
      recursion_total_walltime_(time_duration_t::zero()),
      self_walltime_(time_duration_t::zero()),
      child_walltime_(time_duration_t::zero()),
      overhead_walltime_(time_duration_t::zero()), call_count_(0),
      recursion_level_(0) {

  // If there is a parent region name, use this as a prefix.
  if (!parent_region_name.empty()){
    decorated_region_name_ += parent_region_name;
    decorated_region_name_ += "-->";
  }

  // Add the region name itself.
  decorated_region_name_ += region_name_;

   // Add the call depth in the calling hierarchy.
  if (call_depth != null_call_depth){
    decorated_region_name_ += '~';
    decorated_region_name_ += std::to_string(call_depth);
  }

  // Add the thread ID.
  decorated_region_name_ += '@';
  decorated_region_name_ += std::to_string(tid);
}
