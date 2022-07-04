#include "solution.hpp"

#include <cstdint>
#include <iostream>

inline uint64_t PopCnt(uint64_t n) {
#ifdef _MSC_VER
  return __popcnt64(n);
#else
  return __builtin_popcountll(n);
#endif
}

inline bool ContainsZeroByte(uint64_t v) {
  return ((v - UINT64_C(0x0101010101010101)) & ~v &
          UINT64_C(0x8080808080808080)) != 0;
}

inline uint64_t NLZ(uint64_t x) {
  x = x | (x >> 1);
  x = x | (x >> 2);
  x = x | (x >> 4);
  x = x | (x >> 8);
  x = x | (x >> 16);
  x = x | (x >> 32);

  return PopCnt(~x);
}

inline uint64_t NTZ(uint64_t x) {
  constexpr uint64_t kMod37BitPosition[] = {
      32, 0,  1,  26, 2,  23, 27, 0,  3, 16, 24, 30, 28, 11, 0,  13, 4,  7, 17,
      0,  25, 22, 31, 15, 29, 10, 12, 6, 0,  21, 14, 9,  5,  20, 8,  19, 18};
  if ((x & UINT64_C(0xFFFFFFFF)) == 0) {
    uint32_t v = x >> 32;
    return 32 + kMod37BitPosition[(-v & v) % 37];
  } else {
    uint32_t v = x;
    return kMod37BitPosition[(-v & v) % 37];
  }
}

unsigned solution(const std::string& inputContents) {
  const char* beg = inputContents.c_str();
  const char* end = beg + inputContents.length();
  uint64_t u64beg = reinterpret_cast<uint64_t>(beg);
  uint64_t u64beg_aligned =
      u64beg % 8 == 0 ? u64beg : u64beg + (8 - u64beg % 8);
  uint64_t u64end = reinterpret_cast<uint64_t>(end);
  uint64_t u64end_aligned = u64end - u64end % 8;

  const char* beg_aligned = reinterpret_cast<const char*>(u64beg_aligned);
  ptrdiff_t longest = 0;
  const char* last_line_beg = beg;
  for (const char* ch = beg; ch < beg_aligned; ch++) {
    if (*ch == '\n') {
      longest = std::max<ptrdiff_t>(longest, ch - last_line_beg);
      last_line_beg = ch + 1;
    }
  }

  const uint64_t* pu64_beg = reinterpret_cast<const uint64_t*>(u64beg_aligned);
  const uint64_t* pu64_end = reinterpret_cast<const uint64_t*>(u64end_aligned);

  for (const uint64_t* pu64 = pu64_beg; pu64 < pu64_end; pu64++) {
    constexpr uint64_t kMask = UINT64_C(0x0A0A0A0A0A0A0A0A);
    const uint64_t u64 = *pu64;
    const uint64_t masked = u64 ^ kMask;
    if (!ContainsZeroByte(masked)) {
      continue;
    }
    const uint64_t tmp = ((masked & UINT64_C(0x7F7F7F7F7F7F7F7F)) +
                          UINT64_C(0x7F7F7F7F7F7F7F7F)) &
                         0x8080808080808080;
    const uint64_t x80ed = ~(tmp | masked | UINT64_C(0x7F7F7F7F7F7F7F7F));
    const char* pch = reinterpret_cast<const char*>(pu64);
    ptrdiff_t lsf = pch - last_line_beg;
    if (lsf + 8 >= longest) {
      longest = std::max<ptrdiff_t>(longest, lsf + NTZ(x80ed) / 8);
    }

    last_line_beg = pch + 8 - NLZ(x80ed) / 8;
  }

  const char* end_aligned = reinterpret_cast<const char*>(u64end_aligned);
  for (const char* ch = end_aligned; ch < end; ch++) {
    if (*ch == '\n') {
      longest = std::max<ptrdiff_t>(longest, ch - last_line_beg);
      last_line_beg = ch + 1;
    }
  }

  longest = std::max<ptrdiff_t>(longest, end - last_line_beg);

  return static_cast<unsigned>(longest);
}
