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

# assert 测试

几种 assert
- 带消息断言和不带消息断言
- 函数中断言失败，返回
- 函数中断言失败，继续执行
- 循环中断言失败，继续执行
- 循环中断言失败，中断执行
