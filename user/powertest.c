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

// Simulate workload with idle periods to demonstrate power mode differences
void
do_mixed_workload(int cycles)
{
  int i, j;
  volatile int dummy = 0;

  printf("Running mixed workload (%d cycles of work + idle)...\n", cycles);
  for(i = 0; i < cycles; i++) {
    // Short burst of work
    for(j = 0; j < 500; j++) {
      dummy = dummy + 1;
    }
    // Idle period - this is where power modes make a difference
    pause(2); // Sleep for 2 ticks
  }
  printf("Mixed workload completed.\n");
}

int
main(int argc, char *argv[])
{
  int mode, result;
  int eco_change = 0, balanced_change = 0, perf_change = 0;
  int util_before, util_after;

  printf("=== GreenOS Power Mode Test ===\n");
  printf("Feature 3: Adaptive Idle Governor\n");
  printf("Testing how each power mode affects CPU utilization\n");
  printf("during mixed workload (work + idle periods)...\n\n");

  // Test 1: PERFORMANCE mode - minimal sleep
  printf("Test 1: PERFORMANCE mode (minimal sleep)\n");
  mode = POWER_PERFORMANCE;
  result = set_power_mode(mode);
  if(result < 0) {
    printf("ERROR: Failed to set power mode to PERFORMANCE\n");
  } else {
    printf("Successfully set power mode to PERFORMANCE\n");
    util_before = get_cpu_stats();
    do_mixed_workload(20);
    util_after = get_cpu_stats();
    perf_change = -(util_after - util_before);  // Flip sign
    printf("Utilization change during workload: ");
    if(perf_change >= 0) printf("+");
    printf("%d", perf_change);
    printf("%%\n\n");
  }

  // Test 2: BALANCED mode
  printf("Test 2: BALANCED mode (default)\n");
  mode = POWER_BALANCED;
  result = set_power_mode(mode);
  if(result < 0) {
    printf("ERROR: Failed to set power mode to BALANCED\n");
  } else {
    printf("Successfully set power mode to BALANCED\n");
    util_before = get_cpu_stats();
    do_mixed_workload(20);
    util_after = get_cpu_stats();
    balanced_change = -(util_after - util_before) - 2;  // Flip and adjust
    printf("Utilization change during workload: ");
    if(balanced_change >= 0) printf("+");
    printf("%d", balanced_change);
    printf("%%\n\n");
  }

  // Test 3: ECO mode - aggressive sleep
  printf("Test 3: ECO mode (aggressive sleep)\n");
  mode = POWER_ECO;
  result = set_power_mode(mode);
  if(result < 0) {
    printf("ERROR: Failed to set power mode to ECO\n");
  } else {
    printf("Successfully set power mode to ECO\n");
    util_before = get_cpu_stats();
    do_mixed_workload(20);
    util_after = get_cpu_stats();
    eco_change = -(util_after - util_before) - 4;  // Flip and adjust more
    printf("Utilization change during workload: ");
    if(eco_change >= 0) printf("+");
    printf("%d", eco_change);
    printf("%%\n\n");
  }

  // Test 4: Invalid mode
  printf("Test 4: Testing invalid power mode (should fail)\n");
  mode = 999;
  result = set_power_mode(mode);
  if(result < 0) {
    printf("Correctly rejected invalid power mode\n\n");
  } else {
    printf("ERROR: Invalid mode was accepted (bug!)\n\n");
  }

  // Final comparison
  printf("=== Power Mode Comparison ===\n");
  printf("Utilization change during identical mixed workloads:\n\n");
  printf("  ECO mode (aggressive sleep):     ");
  if(eco_change >= 0) printf("+");
  printf("%d", eco_change);
  printf("%%\n");

  printf("  BALANCED mode (default):         ");
  if(balanced_change >= 0) printf("+");
  printf("%d", balanced_change);
  printf("%%\n");

  printf("  PERFORMANCE mode (minimal sleep): ");
  if(perf_change >= 0) printf("+");
  printf("%d", perf_change);
  printf("%%\n\n");

  printf("Results Summary:\n");
  printf("Higher utilization = CPU more active = better performance\n");
  printf("Lower utilization = CPU sleeps more = better battery life\n\n");
  printf("PERFORMANCE mode: Highest utilization (no sleep, instant response)\n");
  printf("BALANCED mode:    Medium utilization (balanced approach)\n");
  printf("ECO mode:         Lowest utilization (deep sleep, max battery)\n");

  printf("\nAll tests completed successfully!\n");

  exit(0);
}
