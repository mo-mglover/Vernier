/*----------------------------------------------------------------------------*\
 (c) Crown copyright 2026 Met Office. All rights reserved.
 The file LICENCE, distributed with this code, contains details of the terms
 under which the code may be used.
\*----------------------------------------------------------------------------*/

/**
 * @file   vernier_scope.h
 * @brief  Top-level Vernier scope class.
 *
 * Defines the VernierScope class. The constructor and destructor call down to
 * Vernier start and stop callipers. This allows scoped regions to be timed
 * based on object lifetime.
 * 
 * Note: Any time overhead incurred will not be caught by Vernier's internal
 *       self-timing.
 */

 #ifndef VERNIER_SCOPE_H
 #define VERNIER_SCOPE_H

 #include <string_view>

 namespace meto{

 class VernierScope{
    private:
      size_t handle_;

    public:

      // Constructor
      explicit VernierScope(std::string_view);

      // Destructor
      ~VernierScope();

      // Methods
      size_t get_handle();

};
}

#endif
 


