

#ifndef AFFINITY_H
#define AFFINITY_H

#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <sched.h>

#include "mpi_context.h"

#define VERNIER_HIGH_NUM_CPUS_VALUE 9999u

namespace meto{

class Affinity{

  private:

    // Methods
    int max_available_cpus();
    int num_available_cpus();
    int cpumask_weight(cpu_set_t*);
    int running_on_core();
    char hex(int);

  public:
    void write_map(meto::MPIContext&);

};

} // namespace meto

#endif

