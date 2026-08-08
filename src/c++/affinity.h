

#ifndef AFFINITY_H
#define AFFINITY_H

#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <sched.h>
#include <memory>
#include <string>
#ifdef _OPENMP
  #include <omp.h>
#endif

#include "mpi_context.h"

#define VERNIER_HIGH_NUM_CPUS_VALUE 9999u

namespace meto{

// Forward declarations
class AffinitySysCalls;

// Affinity class itsel
class Affinity{

  private:

    // Data members
    std::unique_ptr<AffinitySysCalls> system_calls_;

  public:

    // Constructors
    Affinity();
    explicit Affinity(std::unique_ptr<meto::AffinitySysCalls>);

    char hex(int);
    void write_map(meto::MPIContext&, std::string const&);

};

// System calls
class AffinitySysCalls{

  public:

    virtual ~AffinitySysCalls() = default;

    virtual int max_available_cpus() const = 0;
    virtual int num_available_cpus() const = 0;
    virtual int running_on_core()    const = 0;

};

class MachineAffinitySysCalls : public AffinitySysCalls{

  private:

    int cpumask_weight(cpu_set_t*) const;

  public:

    int max_available_cpus() const override;
    int num_available_cpus() const override;
    int running_on_core()    const override;

};

} // namespace meto

#endif

