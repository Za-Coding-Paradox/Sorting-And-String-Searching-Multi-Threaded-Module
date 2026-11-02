#include "../loader/chunker.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

namespace mth_css {

auto insertionSort(std::vector<int> &bucket, const int bucketSize,
                   DataFrame *df) {
  for (int i{1}; i < bucketSize; ++i) {
    for (int j{0}; j < i; ++j) {
      if (df->ratio[bucket[i - j]] < df->ratio[bucket[i - j - 1]]) {
        std::swap(bucket[i - j], bucket[i - j - 1]);
      } else {
        break; // already in place, everything on its left is sorted
      }
    }
  }
}

auto bucket_sort(DataFrame *df, const auto &dataChunks) -> std::vector<int> {
  // ceil(ratio * 10) gives 0..10 for a ratio in [0, 1], so 11 buckets
  std::vector<std::vector<int>> bucketArray(11, std::vector<int>(0));

  for (int i{0}; i < df->rowCount; ++i) {
    float value = df->ratio[i];
    bucketArray[(int)std::ceil(value * 10)].push_back(i);
  }

  { // jthreads join when this block ends, so every bucket is sorted after it
    std::vector<std::jthread> threadList(NUM_THREADS);

    for (int i{0}; i < NUM_THREADS; ++i) {
      // thread i sorts buckets i, i + NUM_THREADS, i + 2 * NUM_THREADS ...
      // no two threads ever touch the same bucket
      threadList[i] = std::jthread([&, i]() {
        for (int b{i}; b < bucketArray.size(); b += NUM_THREADS) {
          insertionSort(bucketArray[b], bucketArray[b].size(), df);
        }
      });
    }
  }

  // buckets are in ratio order, so joining them end to end gives the result
  std::vector<int> sortedArray;
  for (int i{0}; i < bucketArray.size(); ++i) {
    for (int j{0}; j < bucketArray[i].size(); ++j) {
      sortedArray.push_back(bucketArray[i][j]);
    }
  }

  return sortedArray;
}
} // namespace mth_css
