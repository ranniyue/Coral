add_library(coral_options INTERFACE)

target_compile_options(coral_options INTERFACE
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
    -Wall # 开启基础警告
    -Wextra # 开启额外警告
    -Wpedantic # 严格遵循C++标准警告
    -fno-strict-aliasing # 关闭严格别名规则，规避类型别名带来UB
  >
)
# 生成位置无关代码，编译动态库(.so)必须
target_compile_options(coral_options INTERFACE
  $<$<AND:$<PLATFORM_ID:Linux>,$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>>:
      -fPIC
  >
)
# Linux链接选项：把全部符号加入动态符号表
# 用于dlopen、backtrace栈回溯打印函数名
target_link_options(coral_options INTERFACE
  $<$<AND:$<PLATFORM_ID:Linux>,$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>>:
    -rdynamic
  >
)
# 预定义宏
target_compile_definitions(coral_options INTERFACE
  $<$<CONFIG:Debug>:CORAL_DEBUG>
  $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:NDEBUG>
)

# 编译器优化
target_compile_options(coral_options INTERFACE
  $<$<CONFIG:Debug>:-O0> # 关闭优化，方便调试
  $<$<CONFIG:Debug>:-g3> # 最高等级调试信息，包含宏信息
  $<$<CONFIG:Debug>:-ggdb> # 生成gdb专用调试格式(GCC)

  $<$<CONFIG:Release>:-O2> # O2优化
  $<$<CONFIG:Release>:-fno-omit-frame-pointer> # 保留栈帧指针

  $<$<CONFIG:RelWithDebInfo>:-O2> # O2优化
  $<$<CONFIG:RelWithDebInfo>:-g> # 生成基础调试符号
  $<$<CONFIG:RelWithDebInfo>:-fno-omit-frame-pointer> # 保留栈帧指针
)
