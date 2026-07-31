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
  MPI_Datatype mpi_buffer;
  int record_length;
  int max_record_length;

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

  // Construct the record on each individual rank, and find the maximum length
  // of all of them.
  int num_cpus = num_available_cpus();

  std::ostringstream rss;
  rss << std::setw(8) << std::setfill('0') << mpi_context.get_rank() << " : "
      << mask << " : "
      << std::setw(3) << std::setfill(' ') << num_cpus << "\n";

  record_length = static_cast<int>(rss.str().length());
  MPI_Allreduce(&record_length, &max_record_length, 1, MPI_INT, MPI_MAX,
                mpi_context.get_handle());

  // Having found the maximum record length, pad out the record on each
  // individual MPI rank with spaces.
  rss << std::string(
      static_cast<std::string::size_type>(max_record_length - record_length), ' ');

  MPI_Type_contiguous(max_record_length, MPI_CHAR, &mpi_buffer);
  MPI_Type_commit(&mpi_buffer);

  // Build the header on all ranks for now.
  std::ostringstream hss;
  hss << "--> AFFINITY MAP <--" << "\n\n"
      << "Maximum number of (logical) cores: "
      << max_cpus << "\n\n"
      << "Thread binding map, key:" << "\n\n"
      << "MPI rank"
      << " : ...THREADS..ON..CORES... : "
      << "Num. cores available to threads" << "\n\n"
      << std::string(11, ' ')
      << "Cores ---->" << "\n";
  std::string header = hss.str();
  MPI_Offset header_length = static_cast<MPI_Offset>(header.length());

  // Collective file open
  MPI_File mapfile;
  MPI_File_open(mpi_context.get_handle(), "vernier-affinity-map.txt",
                MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &mapfile);

  // Root writes the header at offset 0
  if (mpi_context.on_root()) {
    MPI_File_write_at(mapfile, 0,
                      header.data(), static_cast<int>(header.size()),
                      MPI_CHARACTER, MPI_STATUS_IGNORE);
  }

  // Each rank writes its own record
  MPI_Offset my_offset = header_length
                       + (static_cast<MPI_Offset>(mpi_context.get_rank())
                          * record_length);

  // Create a view for each task which represents a unique, non-overlapping region.
  MPI_File_set_view(mapfile, my_offset, MPI_CHAR, mpi_buffer, "native", MPI_INFO_NULL);

  MPI_File_write(mapfile, rss.str().c_str(), max_record_length, MPI_CHAR, MPI_STATUS_IGNORE);

  // Close the file collectively.
  MPI_File_close(&mapfile);
  MPI_Type_free(&mpi_buffer);

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
