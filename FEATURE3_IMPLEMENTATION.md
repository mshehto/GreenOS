# GreenOS Feature 3: Adaptive Idle Governor

**Implemented by:** Mohamed Shehto

## Overview

This feature implements an energy-aware adaptive idle governor for xv6, replacing the default busy-wait idle loop with an intelligent power management system that can operate in three different modes.

## Implementation Details

### 1. Power Mode Definitions ([kernel/proc.h](kernel/proc.h))

Added an enum to define three power modes:
- **POWER_ECO (0)**: Aggressive sleep mode for maximum energy savings
- **POWER_BALANCED (1)**: Default mode with moderate sleep behavior
- **POWER_PERFORMANCE (2)**: Minimal sleep for maximum responsiveness

### 2. Global Idle Statistics Tracking ([kernel/proc.c](kernel/proc.c))

Implemented a global `idle_governor` structure that tracks:
- `idle_ticks`: Total CPU ticks spent in idle state
- `active_ticks`: Total CPU ticks spent running processes
- `current_mode`: Currently active power mode

This structure is protected by a spinlock for thread-safe access.

### 3. Intelligent Idle Manager ([kernel/proc.c](kernel/proc.c:489-523))

Modified the scheduler's idle loop to implement power-mode-aware idle behavior:

- **ECO Mode**: Uses three consecutive `wfi` (wait-for-interrupt) instructions to create deeper sleep states, maximizing energy savings
- **BALANCED Mode**: Uses a single `wfi` instruction for standard sleep behavior
- **PERFORMANCE Mode**: No sleep instruction, busy-wait for fastest response time

The scheduler also tracks active vs. idle ticks to measure CPU utilization.

### 4. System Calls

#### `set_power_mode(int mode)` ([kernel/sysproc.c](kernel/sysproc.c:120-137))
Allows userspace programs to change the global power mode.
- Validates the mode parameter (0-2)
- Updates the global power mode atomically
- Returns 0 on success, -1 on invalid mode

#### `get_cpu_stats(void)` ([kernel/sysproc.c](kernel/sysproc.c:140-158))
Returns current CPU utilization as a percentage.
- Calculates: (active_ticks / total_ticks) * 100
- Returns 0 if no data is available yet

### 5. Userspace Interface

Added the following to enable userspace access:
- System call numbers in [kernel/syscall.h](kernel/syscall.h:24-25)
- Function prototypes in [kernel/syscall.c](kernel/syscall.c:105-106)
- Syscall table entries in [kernel/syscall.c](kernel/syscall.c:132-133)
- User library declarations in [user/user.h](user/user.h:28-29)
- Syscall stubs in [user/usys.pl](user/usys.pl:45-46)

### 6. Test Program ([user/powertest.c](user/powertest.c))

Created a comprehensive test program that demonstrates:
- Reading initial CPU utilization
- Switching between all three power modes
- Performing work in each mode
- Measuring CPU utilization differences
- Testing error handling (invalid mode rejection)
- Displaying final statistics

## How to Use

### Running the Test Program

1. Build GreenOS:
   ```bash
   make clean
   make
   ```

2. Run in QEMU:
   ```bash
   make qemu
   ```

3. In the xv6 shell, run:
   ```
   $ powertest
   ```

### Using in Your Own Programs

```c
#include "kernel/types.h"
#include "user/user.h"

#define POWER_ECO 0
#define POWER_BALANCED 1
#define POWER_PERFORMANCE 2

int main() {
    // Switch to eco mode for energy savings
    set_power_mode(POWER_ECO);

    // Do some work...

    // Check CPU utilization
    int util = get_cpu_stats();
    printf("CPU utilization: %d%%\n", util);

    // Switch back to balanced mode
    set_power_mode(POWER_BALANCED);

    exit(0);
}
```

## Key Design Decisions

1. **Global Power Mode**: The power mode is system-wide rather than per-process. This simplifies the implementation and reflects real-world CPU governors.

2. **Three Modes**: Inspired by Linux CPU governors (powersave, ondemand, performance), providing a good balance between simplicity and flexibility.

3. **WFI-based Implementation**: Uses RISC-V's `wfi` (wait-for-interrupt) instruction to simulate low-power states. In real hardware, this would trigger actual power-saving features.

4. **Statistics Tracking**: Maintains running counters of idle vs. active ticks to provide visibility into energy efficiency gains.

5. **Performance Mode Trade-off**: PERFORMANCE mode uses busy-waiting (no sleep) for absolute minimum latency, at the cost of higher energy consumption.

## Performance Characteristics

- **ECO Mode**:
  - Highest energy savings
  - Higher wake-up latency
  - Best for batch workloads

- **BALANCED Mode**:
  - Moderate energy savings
  - Moderate wake-up latency
  - Default mode, good for general use

- **PERFORMANCE Mode**:
  - Minimal energy savings
  - Lowest wake-up latency
  - Best for latency-sensitive workloads

## Files Modified

### Kernel Files
- [kernel/proc.h](kernel/proc.h) - Added power mode enum and proc struct field
- [kernel/proc.c](kernel/proc.c) - Added idle governor and modified scheduler
- [kernel/sysproc.c](kernel/sysproc.c) - Implemented system calls
- [kernel/syscall.h](kernel/syscall.h) - Added syscall numbers
- [kernel/syscall.c](kernel/syscall.c) - Wired up syscalls

### User Files
- [user/user.h](user/user.h) - Added syscall declarations
- [user/usys.pl](user/usys.pl) - Added syscall stubs
- [user/powertest.c](user/powertest.c) - Test program (NEW)

### Build Files
- [Makefile](Makefile) - Added powertest to UPROGS

## Testing

The `powertest` program performs comprehensive testing:
1. ✅ Tests default power mode (BALANCED)
2. ✅ Tests switching to ECO mode
3. ✅ Tests switching to PERFORMANCE mode
4. ✅ Tests switching back to BALANCED mode
5. ✅ Tests invalid mode rejection
6. ✅ Displays CPU utilization statistics

## Future Enhancements

Potential improvements for future versions:
- Per-process power mode hints
- Adaptive mode switching based on load
- Integration with energy profiling (Feature 2)
- More granular idle states
- Power mode transition notifications

## Conclusion

This implementation provides a solid foundation for power management in GreenOS, demonstrating how OS-level scheduling decisions can be made energy-aware while maintaining compatibility with the existing xv6 architecture.
