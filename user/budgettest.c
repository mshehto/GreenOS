#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Test program for GreenOS Feature 4: Energy Budget Enforcement
// Demonstrates setting and enforcing energy budgets on processes

void
cpu_intensive_work(int iterations)
{
  int sum = 0;
  for(int i = 0; i < iterations; i++) {
    for(int j = 0; j < 1000; j++) {
      sum += i * j;
    }
  }
  // Prevent optimization
  if(sum == 0xFFFFFFFF) {
    printf("Done\n");
  }
}

int
main(int argc, char *argv[])
{
  printf("===============================================================================\n");
  printf("GreenOS Energy Budget Enforcement Test\n");
  printf("===============================================================================\n\n");

  int pid = getpid();
  struct energy_info info;

  // Test 1: Get initial energy stats
  printf("Test 1: Initial Energy Stats\n");
  printf("--------------------------------------------------\n");
  if(get_energy_info(pid, &info) == 0) {
    printf("  PID: %d\n", pid);
    printf("  CPU ticks: %d\n", info.cpu_ticks);
    printf("  Energy score: %d\n", info.energy_score);
    printf("  Status: No budget set\n\n");
  } else {
    printf("  Error: Failed to get energy info\n\n");
    exit(1);
  }

  // Test 2: Set a low energy budget and do CPU-intensive work
  printf("Test 2: Setting Low Energy Budget (100 ticks)\n");
  printf("--------------------------------------------------\n");
  int budget = 100;
  if(set_energy_budget(budget) == 0) {
    printf("  Budget set to: %d ticks\n", budget);
  } else {
    printf("  Error: Failed to set energy budget\n\n");
    exit(1);
  }

  // Get current stats before work
  if(get_energy_info(pid, &info) == 0) {
    printf("  Current CPU ticks: %d\n", info.cpu_ticks);
  }

  printf("  Performing CPU-intensive work...\n");

  // Do work - this should trigger budget enforcement
  cpu_intensive_work(5000);

  // Get stats after work
  if(get_energy_info(pid, &info) == 0) {
    printf("  After work CPU ticks: %d\n", info.cpu_ticks);
    printf("  Energy score: %d\n\n", info.energy_score);
  }

  // Test 3: Disable budget and do more work
  printf("Test 3: Disabling Budget (set to 0)\n");
  printf("--------------------------------------------------\n");
  if(set_energy_budget(0) == 0) {
    printf("  Budget disabled\n");
  }

  if(get_energy_info(pid, &info) == 0) {
    printf("  Current CPU ticks: %d\n", info.cpu_ticks);
  }

  printf("  Performing CPU-intensive work without budget...\n");
  cpu_intensive_work(2000);

  if(get_energy_info(pid, &info) == 0) {
    printf("  After work CPU ticks: %d\n", info.cpu_ticks);
    printf("  Energy score: %d\n\n", info.energy_score);
  }

  // Test 4: Set a higher budget
  printf("Test 4: Setting Higher Budget (1000 ticks)\n");
  printf("--------------------------------------------------\n");
  budget = 1000;
  if(set_energy_budget(budget) == 0) {
    printf("  Budget set to: %d ticks\n", budget);
  }

  if(get_energy_info(pid, &info) == 0) {
    printf("  Current CPU ticks: %d\n", info.cpu_ticks);
  }

  printf("  Performing CPU-intensive work...\n");
  cpu_intensive_work(3000);

  if(get_energy_info(pid, &info) == 0) {
    printf("  After work CPU ticks: %d\n", info.cpu_ticks);
    printf("  Energy score: %d\n\n", info.energy_score);
  }

  // Test 5: Test invalid budget
  printf("Test 5: Testing Invalid Budget (-100)\n");
  printf("--------------------------------------------------\n");
  if(set_energy_budget(-100) == -1) {
    printf("  Correctly rejected invalid budget\n\n");
  } else {
    printf("  Error: Invalid budget was accepted!\n\n");
  }

  printf("===============================================================================\n");
  printf("Energy Budget Enforcement Test Complete\n");
  printf("===============================================================================\n");

  exit(0);
}
