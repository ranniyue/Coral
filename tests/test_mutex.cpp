

#include <coral/mutex.h>

#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <latch>
#include <mutex>
#include <thread>
#include <vector>

constexpr int RUNS = 7;

uint64_t g_target = 0;   //
uint64_t g_counter = 0;  // 全局计数

uint64_t OperationsForThread(std::size_t thread_index,
                             std::size_t thread_count) {
  const uint64_t base = g_target / thread_count;
  const uint64_t remainder = g_target % thread_count;
  return base + (thread_index < remainder ? 1 : 0);
}

template <typename Operation>
std::chrono::nanoseconds BenchmarkThreads(std::size_t thread_count,
                                          Operation&& operation) {
  // 线程同步器，让线程同时开始执行
  std::latch ready(static_cast<std::ptrdiff_t>(thread_count));
  std::latch start_gate(1);
  std::latch finished(static_cast<std::ptrdiff_t>(thread_count));

  std::vector<std::thread> threads;
  threads.reserve(thread_count);

  for (std::size_t thread_index = 0; thread_index < thread_count;
       ++thread_index) {
    threads.emplace_back([&, thread_index]() {
      ready.count_down();
      start_gate.wait();
      // 所有线程阻塞到这里等待
      operation(thread_index, OperationsForThread(thread_index, thread_count));
      finished.count_down();
    });
  }

  ready.wait();
  const auto begin = std::chrono::steady_clock::now();
  start_gate.count_down();  // 所有线程开始执行
  finished.wait();          // 所有线程执行结束
  const auto end = std::chrono::steady_clock::now();

  for (std::thread& thread : threads) {
    thread.join();
  }

  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
}

template <typename Mutex>
std::chrono::nanoseconds BenchmarkMutext(std::size_t thread_count) {
  Mutex mutex;
  g_counter = 0;
  const auto elapsed = BenchmarkThreads(
      thread_count, [&mutex](std::size_t, uint64_t operations) {
        for (uint64_t index = 0; index < operations; ++index) {
          std::lock_guard<Mutex> lock(mutex);
          ++g_counter;
        }
      });
  return elapsed;
}

// 按次数取平均
template <typename Benchmark>
double Run(Benchmark&& benchmark) {
  long double total_nanoseconds = 0.0L;
  for (int run = 0; run < RUNS; ++run) {
    total_nanoseconds += static_cast<long double>(benchmark().count());
  }
  return static_cast<double>(total_nanoseconds / RUNS);
}

// 输出结果
void PrintResult(std::string_view name, double total_nanoseconds) {
  const double nanoseconds_pre_operation =
      total_nanoseconds / static_cast<double>(g_target);
  std::cout << std::left << std::setw(30) << name << std::right
            << total_nanoseconds << " ns \ttotal, " << nanoseconds_pre_operation
            << " ns/op\n";
}

class Spinlock {
 public:
  using Lock = std::lock_guard<Spinlock>;
  Spinlock() noexcept = default;
  ~Spinlock() noexcept = default;

  Spinlock(const Spinlock&) = delete;
  Spinlock& operator=(const Spinlock&) = delete;

  void lock() noexcept {
    while (m_mutex.test_and_set(std::memory_order_acquire)) {
      while (m_mutex.test(std::memory_order_relaxed)) {
        cpu_relax();
      }
    }
  }
  [[nodiscard]] bool try_lock() noexcept {
    return !m_mutex.test_and_set(std::memory_order_acquire);
  }
  void unlock() noexcept { m_mutex.clear(std::memory_order_release); }

 private:
  std::atomic_flag m_mutex = ATOMIC_FLAG_INIT;
};

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./test_lock count\n";
    return -1;
  }
  g_target = std::atoi(argv[1]);

  std::cout << std::fixed << std::setprecision(2);
  std::cout << "operations:" << g_target << ", average of " << RUNS
            << " runs\n\n";

  const std::vector<std::size_t> thread_counts = {1, 2, 4, 8, 10};

  for (const std::size_t thread_count : thread_counts) {
    std::cout << "threads:" << thread_count << "\n";
    PrintResult("pthread_spinlock::Spinlock", Run([&]() {
                  return BenchmarkMutext<coral::Spinlock>(thread_count);
                }));

    PrintResult("atomic_flag::Spinlock",
                Run([&]() { return BenchmarkMutext<Spinlock>(thread_count); }));

    PrintResult("std::mutex", Run([&]() {
                  return BenchmarkMutext<std::mutex>(thread_count);
                }));
  }

  return 0;
}
