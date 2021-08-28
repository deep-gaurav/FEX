/*
$info$
tags: LinuxSyscalls|syscalls-x86-32
$end_info$
*/

#include "Tests/LinuxSyscalls/Syscalls.h"
#include "Tests/LinuxSyscalls/x32/Syscalls.h"

#include <bits/types/timer_t.h>
#include <stdint.h>
#include <syscall.h>
#include <unistd.h>

namespace FEXCore::Core {
  struct CpuStateFrame;
}

namespace FEX::HLE::x32 {
  void RegisterTimer() {
    REGISTER_SYSCALL_IMPL_X32(timer_settime64, [](FEXCore::Core::CpuStateFrame *Frame, timer_t timerid, int flags, const struct itimerspec *new_value, struct itimerspec *old_value) -> uint64_t {
      uint64_t Result = ::syscall(SYS_timer_settime, timerid, flags, new_value, old_value);
      SYSCALL_ERRNO();
    });

    REGISTER_SYSCALL_IMPL_X32(timer_gettime64, [](FEXCore::Core::CpuStateFrame *Frame, timer_t timerid, struct itimerspec *curr_value) -> uint64_t {
      uint64_t Result = ::syscall(SYS_timer_gettime, timerid, curr_value);
      SYSCALL_ERRNO();
    });
  }
}
