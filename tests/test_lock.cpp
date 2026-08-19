

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <latch>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

constexpr int RUNS = 7;

uint64_t g_target = 0;   //
uint64_t g_counter = 0;  // 全局计数

std::atomic<uint64_t> g_atomic_counter{0};
std::mutex g_mutex;
std::shared_mutex g_shared_mutex;

// 计算循环次数
uint64_t OperationsForThread(std::size_t thread_index,
                             std::size_t thread_count) {
  const uint64_t base = g_target / thread_count;
  const uint64_t remainder = g_target % thread_count;
  return base + (thread_index < remainder ? 1 : 0);
}

// 按指定的线程数量去执行指定操作
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

// 测试互斥锁
std::chrono::nanoseconds BenchmarkMutext(std::size_t thread_count) {
  g_counter = 0;
  const auto elapsed =
      BenchmarkThreads(thread_count, [](std::size_t, uint64_t operations) {
        for (uint64_t index = 0; index < operations; ++index) {
          std::lock_guard<std::mutex> lock(g_mutex);
          ++g_counter;
        }
      });
  return elapsed;
}
// 测试共享锁
std::chrono::nanoseconds BenchmarkSharedMutex(std::size_t thread_count) {
  g_counter = 0;
  const auto elapsed =
      BenchmarkThreads(thread_count, [](std::size_t, uint64_t operations) {
        for (uint64_t index = 0; index < operations; ++index) {
          std::lock_guard<std::shared_mutex> lock(g_shared_mutex);
          ++g_counter;
        }
      });
  return elapsed;
}
// 测试原子变量
std::chrono::nanoseconds BenchmarkAtomic(std::size_t thread_count) {
  g_atomic_counter.store(0, std::memory_order_relaxed);
  const auto elapsed =
      BenchmarkThreads(thread_count, [](std::size_t, uint64_t operations) {
        for (uint64_t index = 0; index < operations; ++index) {
          g_atomic_counter.fetch_add(1, std::memory_order_relaxed);
        }
      });
  return elapsed;
}
// 测试原子变量自旋
std::chrono::nanoseconds BenchmarkAtomicCas(std::size_t thread_count) {
  g_atomic_counter.store(0, std::memory_order_relaxed);
  const auto elapsed =
      BenchmarkThreads(thread_count, [](std::size_t, uint64_t operations) {
        for (uint64_t index = 0; index < operations; ++index) {
          uint64_t expected = g_atomic_counter.load(std::memory_order_relaxed);
          while (!g_atomic_counter.compare_exchange_weak(
              expected, expected + 1, std::memory_order_relaxed,
              std::memory_order_relaxed)) {
          }
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
  std::cout << std::left << std::setw(14) << name << std::right
            << total_nanoseconds << " ns \ttotal, " << nanoseconds_pre_operation
            << " ns/op\n";
}

// 测试 按1，2，4，8，16线程执行下 锁的性能消耗
int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./test_lock count\n";
    return -1;
  }
  g_target = std::atoi(argv[1]);

  std::cout << std::fixed << std::setprecision(2);
  std::cout << "operations:" << g_target << ", average of " << RUNS
            << " runs\n\n";

  const std::vector<std::size_t> thread_counts = {1, 2, 4, 8, 16};
  for (const std::size_t thread_count : thread_counts) {
    std::cout << "threads:" << thread_count << "\n";
    PrintResult("mutex", Run([&]() { return BenchmarkMutext(thread_count); }));
    PrintResult("sharedmutex",
                Run([&]() { return BenchmarkSharedMutex(thread_count); }));
    PrintResult("atomic", Run([&]() { return BenchmarkAtomic(thread_count); }));
    PrintResult("atomic cas",
                Run([&]() { return BenchmarkAtomicCas(thread_count); }));
  }
  return 0;
}
