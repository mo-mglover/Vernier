/*------------------------------------------------------------------------------
 * C-language routines to report affinity information.
 *
 *------------------------------------------------------------------------------
 */

#include <fstream>
#include <sched.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <omp.h>

#include "affinity.h"
#include "vernier_mpi.h"

/*------------------------------------------------------------------------------
* SYNOPSIS
*   int max_available_cpus_C()
*
* DESCRIPTION
*   Returns the maximum number of cores (real and virtual) available on a node.
*-------------------------------------------------------------------------------
*/

int meto::Affinity::max_available_cpus()
{
  int max_cpus=VERNIER_HIGH_NUM_CPUS_VALUE;
  max_cpus = static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));
  return max_cpus;
}

/*------------------------------------------------------------------------------
* SYNOPSIS
*   int num_available_cpus()
*
* DESCRIPTION
*   Returns the number of cores (real and virtual) on which this thread may run.
*   This number will be unity if a thread is bound to run on a single (logical)
*   core.
*-------------------------------------------------------------------------------
*/

int meto::Affinity::num_available_cpus()
{
  {
    int num_cpus;
    cpu_set_t cpumask;
    pid_t tid;

    tid = static_cast<pid_t>(syscall(SYS_gettid));
    CPU_ZERO(&cpumask);

    sched_getaffinity(tid, sizeof(cpu_set_t), &cpumask);

    num_cpus = cpumask_weight(&cpumask);

    return num_cpus;
  }
}

/*------------------------------------------------------------------------------
* SYNOPSIS
*   int cpumask_weight()
*
* DESCRIPTION
*   Returns the number of elements of the cpumask that are set.
*-------------------------------------------------------------------------------
*/

int meto::Affinity::cpumask_weight(cpu_set_t * cpumask)
{
  int weight;
  int index;

  weight=0;
  for (index=0; index < CPU_SETSIZE; ++index){
    if (CPU_ISSET( static_cast<size_t>(index), cpumask)){weight++;}
  }

  return weight;
}

/*------------------------------------------------------------------------------
* SYNOPSIS
*   int running_on_core()
*
* DESCRIPTION
*   Returns the ID of the core on which the calling task/thread is running.
*-------------------------------------------------------------------------------
*/

int meto::Affinity::running_on_core()
{
  int core=VERNIER_HIGH_NUM_CPUS_VALUE;
  core = sched_getcpu();
  return core;
}


/*-------------------------------------------------------------------------------
* SYNOPSIS
*   call write_map(funit, comm_size, my_rank, writer_rank)
*
* DESCRIPTION
*   Writes affinitisation in ASCII-art form.
*
* ARGUMENTS
*   funit       -- The file unit to write to.
*   comm_size   -- The number of ranks in the MPI communicator.
*   rank        -- The rank of this particular MPI task.
*   writer_rank -- The rank of the MPI task doing the writing.
*-------------------------------------------------------------------------------
*/

void meto::Affinity::write_map(meto::MPIContext& mpi_context)
{

  // Internal variables
  MPI_Status recv_status;
  MPI_Status send_status;
  MPI_Request send_request;
  // MPI_Datatype mpi_buffer;

  std::size_t max_cpus = static_cast<std::size_t>(max_available_cpus());
  std::string mask(max_cpus, '.');

  #pragma omp parallel default(none) shared(mask)
    {

      int thread_id=0;
      #ifdef _OPENMP
      thread_id = omp_get_thread_num();
      #endif

      int core_id = running_on_core();
      char hex_char = hex(thread_id);

      // If more than one thread is running on the same core, show that with a
      // hash symbol.
      #pragma omp critical
        {
          if (mask.at(core_id) == '.'){mask.at(core_id) = hex_char;}
          else                        {mask.at(core_id) = '#';}
        } // critical
    } // parallel

    int num_cpus = num_available_cpus();
    std::ostringstream oss;
    oss << std::setw(8) << std::setfill('0') << mpi_context.get_rank() << " : "
        << mask << " : "
        << num_cpus;

    // Ensure send and receive buffers have the same length.
    std::string send_buffer = oss.str();
    std::string recv_buffer(send_buffer.length(), '+');

    // Everyone sends to root
    MPI_Isend(send_buffer.data(), send_buffer.length(), MPI_CHARACTER,
                   0, mpi_context.get_rank(),
                   mpi_context.get_handle(), &send_request);

  // Writer receives and writes to file
  if(mpi_context.on_root()) {

    std::ofstream mapfile("vernier-affinity-map.txt");

    mapfile << "--> AFFINITY MAP <--" << "\n\n";

    // Print maximum of logical cores
    mapfile << "Maximum number of (logical) cores: "
            << max_cpus << "\n\n";

    // Thread affinity map
    mapfile << "Thread binding map, key:" << "\n\n";

    mapfile << "MPI rank"
            << " : ...THREADS..ON..CORES... : "
            << "Num. cores available to threads" << "\n\n";
    mapfile << std::string(11, ' ')
            << "Cores ---->" << "\n";

    for (int irank=0; irank< mpi_context.get_size() ; ++irank)
      {
        MPI_Recv(recv_buffer.data(), recv_buffer.length(), MPI_CHARACTER,
                 irank, irank,
                 mpi_context.get_handle(), &recv_status);
        mapfile << recv_buffer << "\n";
      }
    mapfile << std::endl;

    // Close the file
    mapfile.close();
  }

  // Send completed?
  MPI_Wait(&send_request, &send_status);

}

/*-------------------------------------------------------------------------------
* SYNOPSIS
*   hex(num)
*
* DESCRIPTION
*   Generates the single-digit thread ID.
*
*-------------------------------------------------------------------------------
*/

char meto::Affinity::hex(int num)
{

  char digit;

  if      (num < 10) {digit = static_cast<char>('0' + num);}        //  10 numerical digits
  else if (num < 36) {digit = static_cast<char>('a' + (num-10));}   // +26 lowercase digits
  else if (num < 62) {digit = static_cast<char>('A' + (num-36));}   // +26 uppercase digits
  else               {digit = '~';}

  return digit;

}
