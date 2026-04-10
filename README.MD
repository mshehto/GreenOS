# GreenOS - Energy-Aware Operating System

An energy-aware extension of xv6-riscv with power management features.

## Prerequisites

- RISC-V toolchain (`riscv64-unknown-elf-gcc` or similar)
- QEMU 7.2+ for RISC-V (`qemu-system-riscv64`)
- Standard build tools (`make`, `gcc`, `perl`)

## Build and Run

```bash
make clean          # Clean previous builds
make                # Build kernel and programs
make qemu           # Run in QEMU (Ctrl-A then X to exit)
```

## Testing GreenOS Features

After booting to the shell (`$`), run these test programs:

```bash
$ energytest        # Test energy profiling (CPU ticks, energy scores)
$ energytop         # Monitor energy usage of all processes
$ powertest         # Test ECO/BALANCED/PERFORMANCE power modes
$ budgettest        # Test energy budget enforcement
$ usertests         # Standard xv6 test suite
```

## Expected Results

- **energytest**: Shows increasing CPU ticks and energy scores
- **energytop**: Displays real-time process energy metrics
- **powertest**: Different energy consumption across power modes
- **budgettest**: Process throttled when exceeding budget

## Troubleshooting

- **Missing toolchain**: Install RISC-V GCC or use `make TOOLPREFIX=riscv64-linux-gnu-`
- **QEMU version error**: Upgrade to QEMU 7.2+
- **Build fails**: Run `make clean` first

## Quick Test Checklist

- [ ] `make` builds successfully
- [ ] `make qemu` boots to shell
- [ ] All test programs (`energytest`, `energytop`, `powertest`, `budgettest`) run
- [ ] `usertests` passes
