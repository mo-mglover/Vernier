/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2026 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

#include "vernier_scope.h"
#include "vernier.h"

/**
 * @brief Constructor for VernierScope class.
 * @param [in]  region_name   The name of the region to measure.
 * @details  Calls the Vernier start calliper to begin timing the named region
 *           on instantiation.
 */

meto::VernierScope::VernierScope(std::string_view region_name){
  handle_ = meto::vernier.start(region_name);
}

/**
 * @brief Destructor for VernierScope class.
 * @details  Calls the Vernier stop calliper to stop timing the
 *           region when the object goes out of scope.
 */

meto::VernierScope::~VernierScope(){
  meto::vernier.stop(handle_);
}

/**
 * @brief Returns the handle for the region.
 * @returns Region hash.
 */

 size_t meto::VernierScope::get_handle(){
  return handle_;
 }

