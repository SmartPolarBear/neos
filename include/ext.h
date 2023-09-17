//
// Created by bear on 8/24/2023.
//

#pragma once

#define NO_RETURN __attribute__((noreturn))
#define UNREACHABLE __builtin_unreachable()
#define ALWAYS_INLINE __attribute__((always_inline))
#define PACKED __attribute__((packed))
#define ALIGN(x) __attribute__((aligned(x),packed))
#define NONNULL __attribute__((nonnull))