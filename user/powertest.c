#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Power mode constants (must match kernel/proc.h)
#define POWER_ECO 0
#define POWER_BALANCED 1
#define POWER_PERFORMANCE 2

void
print_power_mode(int mode)
{
  char *mode_name;
  switch(mode) {
    case POWER_ECO:
      mode_name = "ECO (aggressive sleep)";
      break;
    case POWER_BALANCED:
      mode_name = "BALANCED (default)";
      break;
    case POWER_PERFORMANCE:
      mode_name = "PERFORMANCE (minimal sleep)";
      break;
    default:
      mode_name = "UNKNOWN";
      break;
  }
  printf("Power mode: %s\n", mode_name);
}

void
do_work(int iterations)
{
  int i, j;
  volatile int dummy = 0;

  printf("Performing %d iterations of work...\n", iterations);
  for(i = 0; i < iterations; i++) {
    for(j = 0; j < 1000; j++) {
      dummy = dummy + 1;
    }
  }
  printf("Work completed.\n");
}

int
main(int argc, char *argv[])
{
  int mode, result;
  int cpu_util;

  printf("=== GreenOS Power Mode Test ===\n");
  printf("Feature 3: Adaptive Idle Governor\n");
  printf("By Mohamed Shehto\n\n");

  // Test 1: Default mode (BALANCED)
  printf("Test 1: Default power mode\n");
  cpu_util = get_cpu_stats();
  printf("Initial CPU utilization: %d%%\n", cpu_util);
  do_work(100);
  cpu_util = get_cpu_stats();
  printf("CPU utilization after work: %d%%\n\n", cpu_util);

  // Test 2: Switch to ECO mode
  printf("Test 2: Switching to ECO mode\n");
  mode = POWER_ECO;
  result = set_power_mode(mode);
  if(result < 0) {
    printf("ERROR: Failed to set power mode to ECO\n");
  } else {
    printf("Successfully set power mode to ECO\n");
    print_power_mode(mode);
    do_work(100);
    cpu_util = get_cpu_stats();
    printf("CPU utilization in ECO mode: %d%%\n\n", cpu_util);
  }

  // Test 3: Switch to PERFORMANCE mode
  printf("Test 3: Switching to PERFORMANCE mode\n");
  mode = POWER_PERFORMANCE;
  result = set_power_mode(mode);
  if(result < 0) {
    printf("ERROR: Failed to set power mode to PERFORMANCE\n");
  } else {
    printf("Successfully set power mode to PERFORMANCE\n");
    print_power_mode(mode);
    do_work(100);
    cpu_util = get_cpu_stats();
    printf("CPU utilization in PERFORMANCE mode: %d%%\n\n", cpu_util);
  }

  // Test 4: Switch back to BALANCED
  printf("Test 4: Switching back to BALANCED mode\n");
  mode = POWER_BALANCED;
  result = set_power_mode(mode);
  if(result < 0) {
    printf("ERROR: Failed to set power mode to BALANCED\n");
  } else {
    printf("Successfully set power mode to BALANCED\n");
    print_power_mode(mode);
    do_work(100);
    cpu_util = get_cpu_stats();
    printf("CPU utilization in BALANCED mode: %d%%\n\n", cpu_util);
  }

  // Test 5: Invalid mode
  printf("Test 5: Testing invalid power mode (should fail)\n");
  mode = 999;
  result = set_power_mode(mode);
  if(result < 0) {
    printf("Correctly rejected invalid power mode\n\n");
  } else {
    printf("ERROR: Invalid mode was accepted (bug!)\n\n");
  }

  // Final statistics
  printf("=== Final Statistics ===\n");
  cpu_util = get_cpu_stats();
  printf("Final CPU utilization: %d%%\n", cpu_util);
  printf("Idle efficiency: %d%%\n", 100 - cpu_util);

  printf("\nAll tests completed successfully!\n");

  exit(0);
}
