file(GLOB_RECURSE CORAL_SOURCE CONFIGURE_DEPENDS *.cpp *.cc *.c)
file(GLOB_RECURSE CORAL_HEADER CONFIGURE_DEPENDS *.h *.hpp)

add_library(coral STATIC ${CORAL_SOURCE} ${CORAL_HEADER})

target_include_directories(coral
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 继承 coral_options 配置
target_link_libraries(coral PUBLIC coral_options)

# 单独配置
target_compile_options(coral PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
    -Werror # 将警告视为错误
  >
)

# 导出设置
install(TARGETS coral
  EXPORT coralTargets # 将该目标加入 coralTargets 导出集合
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} #静态库 .a / .lib
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # 动态库 .so / .dylib
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # Windows dll，放到 bin 目录
)

install(FILES ${CORAL_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/coral
)
