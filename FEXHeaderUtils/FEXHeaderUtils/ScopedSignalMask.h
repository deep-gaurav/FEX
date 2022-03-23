#pragma once

#include <FEXCore/Utils/CompilerDefs.h>
#include <FEXCore/Utils/LogManager.h>

#include <cstdint>
#include <mutex>
#include <shared_mutex>
#include <signal.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace FHU {
  /**
   * @brief A class that masks signals and locks a mutex until it goes out of scope
   *
   * Constructor order:
   * 1) Mask signals
   * 2) Lock Mutex
   *
   * Destructor Order:
   * 1) Unlock Mutex
   * 2) Unmask signals
   */
  class ScopedSignalMaskWithMutex final {
    public:
      ScopedSignalMaskWithMutex(std::mutex &_Mutex, uint64_t Mask = ~0ULL)
        : Mutex {_Mutex} {
        // Mask all signals, storing the original incoming mask
        ::syscall(SYS_rt_sigprocmask, SIG_SETMASK, &Mask, &OriginalMask, sizeof(OriginalMask));

        // Lock the mutex
        Mutex.lock();
      }

      ~ScopedSignalMaskWithMutex() {
        // Unlock the mutex
        Mutex.unlock();

        // Unmask back to the original signal mask
        ::syscall(SYS_rt_sigprocmask, SIG_SETMASK, &OriginalMask, nullptr, sizeof(OriginalMask));
      }
    private:
      uint64_t OriginalMask{};
      std::mutex &Mutex;
  };

  class ScopedSignalMaskWithSharedMutex final {
    public:
      ScopedSignalMaskWithSharedMutex(std::shared_mutex &_Mutex, bool _Shared, uint64_t Mask = ~0ULL)
        : Mutex {_Mutex}, Shared(_Shared) {
        // Mask all signals, storing the original incoming mask
        ::syscall(SYS_rt_sigprocmask, SIG_SETMASK, &Mask, &OriginalMask, sizeof(OriginalMask));

        // Lock the mutex
        if (Shared) {
          Mutex.lock_shared();
        } else {
          Mutex.lock();
        }
      }

      ~ScopedSignalMaskWithSharedMutex() {
        // Unlock the mutex
        if (Shared) {
          Mutex.unlock_shared();
        } else {
          Mutex.unlock();
        }

        // Unmask back to the original signal mask
        ::syscall(SYS_rt_sigprocmask, SIG_SETMASK, &OriginalMask, nullptr, sizeof(OriginalMask));
      }
    private:
      std::shared_mutex &Mutex;
      uint64_t OriginalMask{};
      bool Shared;
  };
}
