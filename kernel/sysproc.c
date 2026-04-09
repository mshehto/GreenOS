#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// GreenOS: Feature 3 - Adaptive Idle Governor
// System call to set the global power mode
extern struct {
  struct spinlock lock;
  uint64 idle_ticks;
  uint64 active_ticks;
  int current_mode;
} idle_governor;

uint64
sys_set_power_mode(void)
{
  int mode;
  argint(0, &mode);

  // Validate the power mode
  if(mode < 0 || mode > 2) {
    return -1; // Invalid mode
  }

  // Set the global power mode
  acquire(&idle_governor.lock);
  idle_governor.current_mode = mode;
  release(&idle_governor.lock);

  return 0; // Success
}

// GreenOS: Feature 3 - Get CPU utilization statistics
uint64
sys_get_cpu_stats(void)
{
  uint64 idle, active, total, utilization;

  acquire(&idle_governor.lock);
  idle = idle_governor.idle_ticks;
  active = idle_governor.active_ticks;
  release(&idle_governor.lock);

  total = idle + active;
  if(total == 0) {
    return 0; // No data yet
  }

  // Return utilization percentage (active / total * 100)
  utilization = (active * 100) / total;
  return utilization;
}

// GreenOS: Feature 2 - Process Energy Profiler
// System call to get energy profiling information for a process
extern struct proc proc[NPROC];

uint64
sys_get_energy_info(void)
{
  int pid;
  uint64 info_addr;
  struct proc *p;

  // Get arguments: pid and pointer to info struct
  argint(0, &pid);
  argaddr(1, &info_addr);

  // Find the process with the given pid
  for(p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if(p->pid == pid) {
      // Update energy score before returning
      update_energy_score(p);

      // Copy energy data to user space
      // We'll copy 4 integers: cpu_ticks, sleep_count, wake_count, energy_score
      int data[4];
      data[0] = p->cpu_ticks;
      data[1] = p->sleep_count;
      data[2] = p->wake_count;
      data[3] = p->energy_score;

      release(&p->lock);

      // Copy to user space
      if(copyout(myproc()->pagetable, info_addr, (char *)data, sizeof(data)) < 0) {
        return -1;
      }
      return 0; // Success
    }
    release(&p->lock);
  }

  return -1; // Process not found
}

// GreenOS: Feature 4 - Energy Budget Enforcement
// System call to set energy budget for the calling process
uint64
sys_set_energy_budget(void)
{
  int budget;
  argint(0, &budget);

  // Validate the budget (must be positive or 0 to disable)
  if(budget < 0) {
    return -1; // Invalid budget
  }

  struct proc *p = myproc();
  acquire(&p->lock);
  p->energy_budget = budget;
  p->energy_used = p->cpu_ticks; // Start tracking from current usage
  release(&p->lock);

  return 0; // Success
}
