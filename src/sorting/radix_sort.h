#include "../loader/chunker.h"
#include <algorithm>
#include <iostream>
#include <vector>

namespace mth_css {
// reads the buckets back into radixArray in order (first bucket to last) and
// empties them, shared by the digit and the byte versions
auto drainBuckets(std::vector<int> &radixArray,
                  std::vector<std::vector<int>> &radixBucket) -> void {
  radixArray.clear();
  for (int b{0}; b < radixBucket.size(); ++b) {
    for (int j{0}; j < radixBucket[b].size(); ++j) {
      radixArray.push_back(radixBucket[b][j]);
    }
    radixBucket[b].clear();
  }
}

// TODO: Fix the byte Radix sort
auto byteRadixSort(std::vector<int> &radixArray,
                   std::vector<std::vector<int>> radixBucket, DataFrame *df) {

  // the largest value tells us how many digits (passes) we need
  long long maxValue = 0;
  for (int i{0}; i < df->rowCount; ++i) {
    maxValue = std::max(maxValue, df->timestamp[i]);
  }
  int maxBytes = 0;
  for (long long m = maxValue; m > 0; m >> 8) {
    maxBytes++;
  }

  for (int i{0}; i < df->rowCount; ++i) {
    long long value = df->timestamp[i];
    int radix = value ^ 0xFF;

    radixBucket[radix].push_back(i);
  }

  for (int i{0}; i < maxBytes - 1; ++i) {

    drainBuckets(radixArray, radixBucket);

    for (int j{0}; j < radixArray.size(); ++j) {
      long long value = df->timestamp[radixArray[j]];
      int radix = (value >> (8 * (i + 1))) ^ 0xFF;
      radixBucket[radix].push_back(radixArray[j]);
    }
  }

  drainBuckets(radixArray, radixBucket);
  return radixArray;
}

// base is 10 (digit by digit) or 256 (byte by byte)
auto radixSort(int base, DataFrame *df) -> std::vector<int> {

  std::vector<int> radixArray(df->rowCount);
  std::vector<std::vector<int>> radixBucket(base, std::vector<int>(0));

  if (base > 10) {
    return byteRadixSort(radixArray, radixBucket, df);
  }

  // the largest value tells us how many digits (passes) we need
  long long maxValue = 0;
  for (int i{0}; i < df->rowCount; ++i) {
    maxValue = std::max(maxValue, df->timestamp[i]);
  }
  int maxDigits = 0;
  for (long long m = maxValue; m > 0; m /= 10) {
    maxDigits++;
  }

  for (int i{0}; i < df->rowCount; ++i) {
    long long value = df->timestamp[i];
    int radix = value % 10;

    radixBucket[radix].push_back(i);
  }

  long long modulo = 100;
  for (int i{0}; i < maxDigits - 1; ++i) {

    drainBuckets(radixArray, radixBucket);

    for (int j{0}; j < radixArray.size(); ++j) {
      long long value = df->timestamp[radixArray[j]];
      int radix = (value % modulo) / (modulo / 10);
      radixBucket[radix].push_back(radixArray[j]);
    }

    modulo *= 10;
  }

  drainBuckets(radixArray, radixBucket);
  return radixArray;
}
} // namespace mth_css
