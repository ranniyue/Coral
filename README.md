Coral‑net 是基于 C++20 协程实现的高性能网络内核。
采用多 Reactor 多线程模型，连接负载均衡，以极低开销支撑海量 TCP 并发。
项目名取自珊瑚分叉丛生的形态，象征多 Reactor 实例并行工作，承载大量网络连接。
轻量、头文件友好，面向 Linux 平台，用于构建高吞吐网络服务。

# 构建

构建 debug
`cmake -B build`
`cmake --build build`

构建 release
`cmake -B build -DCMAKE_BUILD_TYPE=Release`
`cmake --build build`

# assert

几种 assert
- 带消息断言和不带消息断言
- 函数中断言失败，返回
- 函数中断言失败，继续执行
- 循环中断言失败，继续执行
- 循环中断言失败，中断执行

测试断言：test_macros.cpp

# 多线程

线程关系：互斥和同步
互斥->竞争关系
- 抢占同一个独占资源
- 谁拿到谁用，其他人等待
- 实现方法：锁
- 目标：保护资源
同步->协作关系
- 按顺序配合完成一件事
- 生产消费模型
- 实现方法：信号量/条件变量
- 目标：协调顺序

互斥锁：
std::mutex 基础独占锁
- 同意时刻，线程独占
std::recursive_mutex 递归锁
- 允许同一线程重复加锁，谨慎使用
std::timed_mutex 超时锁
- try_lock_for/try_lock_until，等待超时
std::shared_mutex 读写锁
- 读锁多线程共享，写锁独占

信号量：
std::counting_semaphore 计数信号量
- 控制可用资源量
std::binary_semaphore 二元信号量
- 线程通知
std::automic_flag 原子标记
- 唯一保证无锁的原子类型

辅助加锁工具：
std::lock_guard ARII锁
- 构造时加锁，离开作用域解锁
std::unique_lock 灵活锁
- 支持手动解锁，延迟加锁，移动语义
std::scoped_lock 多锁同时获取
- 一次锁定多个变量

测试锁和自选的性能：test_lock.cpp
`cmake -B build -DCMAKE_BUILD_TYPE=Release`
`cmake --build build`
