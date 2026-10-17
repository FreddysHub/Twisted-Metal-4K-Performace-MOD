#pragma once
#include <cstdint>

namespace tm4k_timing
{
// RSX tags count operations, not milliseconds or display frames. Menu/UI blits
// can exceed the original 600-operation bound even with a completed cohort.
// Keep a finite process-local test window and reject future/empty producers.
constexpr bool completed_color_eligible(std::uint64_t color_tag,
    std::uint64_t display_tag, std::uint64_t max_gap)
{
    return color_tag && color_tag <= display_tag && display_tag - color_tag < max_gap;
}
}
