#ifndef CORAL_INCLUDE_MACROS_H
#define CORAL_INCLUDE_MACROS_H

#include <cassert>
#include <cstdint>
#include <iostream>

#define CORAL_ASSERT(x)                                                  \
  if (!(x)) [[unlikely]] {                                               \
    std::cerr << __FILE__ << ":" << __LINE__ << " Assert Failed: " << #x \
              << ".\nStacktrace: to do \n";                              \
    assert(x);                                                           \
  }

#define CORAL_ASSERT_INFO(x, w)                                          \
  if (!(x)) [[unlikely]] {                                               \
    std::cerr << __FILE__ << ":" << __LINE__ << " Assert Failed: " << #x \
              << " [" << w << "].\nStacktrace: to do \n";                \
    assert(x);                                                           \
  }

#define ASSERT_RETVAL(x, val) \
  do {                        \
    if (x) [[likely]]         \
      break;                  \
    CORAL_ASSERT(x);          \
    return val;               \
  } while (0)

#define ASSERT_RETVAL_INFO(x, val, info) \
  do {                                   \
    if (x) [[likely]]                    \
      break;                             \
    CORAL_ASSERT_INFO(x, info);          \
    return val;                          \
  } while (0)

#define ASSERT_RETNONE(x) \
  do {                    \
    if (x) [[likely]]     \
      break;              \
    CORAL_ASSERT(x);      \
    return;               \
  } while (0)

#define ASSERT_RETNONE_INFO(x, info) \
  do {                               \
    if (x) [[likely]]                \
      break;                         \
    CORAL_ASSERT_INFO(x, info);      \
    return;                          \
  } while (0)

#define ASSERT_NOEFFECT(x) \
  do {                     \
    if (x) [[likely]]      \
      break;               \
    CORAL_ASSERT(x);       \
  } while (0)

#define ASSERT_NOEFFECT_INFO(x, info) \
  do {                                \
    if (x) [[likely]]                 \
      break;                          \
    CORAL_ASSERT_INFO(x, info);       \
  } while (0)

#define ASSERT_CONTINUE(x) \
  do {                     \
    if (x) [[likely]]      \
      break;               \
    CORAL_ASSERT(x);       \
    continue;              \
  } while (0)

#define ASSERT_CONTINUE_INFO(x, info) \
  do {                                \
    if (x) [[likely]]                 \
      break;                          \
    CORAL_ASSERT_INFO(x, info);       \
    continue;                         \
  } while (0)

#define ASSERT_BREAK(x)    \
  if (!(x)) [[unlikely]] { \
    CORAL_ASSERT(x);       \
    break;                 \
  } else {                 \
  }

#define ASSERT_BREAK_INFO(x, info) \
  if (!(x)) [[unlikely]] {         \
    CORAL_ASSERT_INFO(x, info);    \
    break;                         \
  } else {                         \
  }

#define INVALID64 0xFFFFFFFFFFFFFFFF
#define INVALID32 0xFFFFFFFF
#define INVALID16 0xFFFF
#define INVALID8 0xFF

#define MAX_U8 0xFF
#define MAX_U16 0xFFFF
#define MAX_U32 0xFFFFFFFF
#define MAX_U64 0xFFFFFFFFFFFFFFFF

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

#endif
