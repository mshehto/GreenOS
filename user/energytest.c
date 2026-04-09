#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Simple test program for energy profiler feature

int
main(int argc, char *argv[])
{
  struct energy_info info;
  int pid = getpid();
  int i, j;

  printf("Energy Profiler Test Program\n");
  printf("============================\n\n");

  // Get initial energy info
  if(get_energy_info(pid, &info) == 0) {
    printf("Initial energy stats for PID %d:\n", pid);
    printf("  CPU ticks:    %d\n", info.cpu_ticks);
    printf("  Sleep count:  %d\n", info.sleep_count);
    printf("  Wake count:   %d\n", info.wake_count);
    printf("  Energy score: %d\n\n", info.energy_score);
  } else {
    printf("Failed to get energy info!\n");
    exit(1);
  }

  // Do some CPU-intensive work
  printf("Doing CPU-intensive work...\n");
  for(i = 0; i < 1000; i++) {
    for(j = 0; j < 1000; j++) {
      // Busy work
      asm volatile("nop");
    }
  }

  // Get energy info after CPU work
  if(get_energy_info(pid, &info) == 0) {
    printf("\nEnergy stats after CPU work:\n");
    printf("  CPU ticks:    %d\n", info.cpu_ticks);
    printf("  Sleep count:  %d\n", info.sleep_count);
    printf("  Wake count:   %d\n", info.wake_count);
    printf("  Energy score: %d\n\n", info.energy_score);
  }

  // Do some I/O work (which will cause sleeps)
  printf("Doing I/O work (writing to console)...\n");
  for(i = 0; i < 10; i++) {
    printf(".");
    pause(1); // Sleep for a timer tick
  }
  printf("\n");

  // Get final energy info
  if(get_energy_info(pid, &info) == 0) {
    printf("\nFinal energy stats after I/O work:\n");
    printf("  CPU ticks:    %d\n", info.cpu_ticks);
    printf("  Sleep count:  %d\n", info.sleep_count);
    printf("  Wake count:   %d\n", info.wake_count);
    printf("  Energy score: %d\n\n", info.energy_score);
  }

  // Test getting energy info for non-existent PID
  printf("Testing error handling (PID 9999)...\n");
  if(get_energy_info(9999, &info) < 0) {
    printf("Correctly returned error for non-existent PID\n");
  } else {
    printf("ERROR: Should have failed for non-existent PID!\n");
  }

  printf("\n============================\n");
  printf("Energy Profiler Test Complete!\n");
  exit(0);
}
