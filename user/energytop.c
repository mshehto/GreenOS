#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Display energy profiling information for all processes
// Similar to Linux 'top' command but showing energy metrics

#define MAX_PROC 64

void
print_header(void)
{
  printf("===============================================================================\n");
  printf("ENERGYTOP - GreenOS Process Energy Profiler\n");
  printf("===============================================================================\n");
  printf("%-6s %-16s %-10s %-10s %-10s %-12s\n",
         "PID", "NAME", "CPU_TICKS", "SLEEPS", "WAKES", "ENERGY_SCORE");
  printf("-------------------------------------------------------------------------------\n");
}

void
print_footer(void)
{
  printf("===============================================================================\n");
  printf("Press Ctrl+C to exit\n");
}

int
main(int argc, char *argv[])
{
  struct energy_info info;
  int pid;
  int count = 0;

  print_header();

  // Try all possible PIDs (1 to MAX_PROC)
  // In xv6, PIDs are typically small numbers
  for(pid = 1; pid < MAX_PROC; pid++) {
    // Try to get energy info for this PID
    if(get_energy_info(pid, &info) == 0) {
      // Process exists and we got its info
      // Note: We can't easily get the process name from userspace in xv6
      // without additional syscalls, so we'll just show the PID
      printf("%-6d %-16s %-10d %-10d %-10d %-12d\n",
             pid,
             "(unknown)",  // Process name not available without additional syscall
             info.cpu_ticks,
             info.sleep_count,
             info.wake_count,
             info.energy_score);
      count++;
    }
  }

  if(count == 0) {
    printf("No processes found.\n");
  } else {
    printf("-------------------------------------------------------------------------------\n");
    printf("Total processes: %d\n", count);
  }

  print_footer();
  exit(0);
}
