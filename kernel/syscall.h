// System call numbers
#define SYS_fork    1
#define SYS_exit    2
#define SYS_wait    3
#define SYS_pipe    4
#define SYS_read    5
#define SYS_kill    6
#define SYS_exec    7
#define SYS_fstat   8
#define SYS_chdir   9
#define SYS_dup    10
#define SYS_getpid 11
#define SYS_sbrk   12
#define SYS_pause  13
#define SYS_uptime 14
#define SYS_open   15
#define SYS_write  16
#define SYS_mknod  17
#define SYS_unlink 18
#define SYS_link   19
#define SYS_mkdir  20
#define SYS_close  21
// GreenOS: Feature 3 - Adaptive Idle Governor
#define SYS_set_power_mode 22
#define SYS_get_cpu_stats  23
// GreenOS: Feature 2 - Process Energy Profiler
#define SYS_get_energy_info 24
// GreenOS: Feature 4 - Energy Budget Enforcement
#define SYS_set_energy_budget 25
