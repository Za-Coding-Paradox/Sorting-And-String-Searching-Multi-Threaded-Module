#include "../loader/chunker.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <limits>
#include <vector>

namespace mth_css {
auto countSort(DataFrame *df, const auto &dataChunks) -> std::vector<int> {
  int numThreads = std::min(NUM_THREADS, (int)dataChunks.size());
  int numChunksPerThread = std::max(1, (int)dataChunks.size() / NUM_THREADS);

  int cummulativeSum[5] = {0};
  std::vector<std::vector<int>> localSum(numThreads, std::vector<int>(5, 0));
  std::vector<std::vector<int>> localFrequency(numThreads,
                                               std::vector<int>(5, 0));

  { // threads join when this block ends
    std::vector<std::jthread> threadList(numThreads);

    for (int i{0}; i < numThreads; i++) {
      threadList[i] = std::jthread([&, i]() {
        int firstChunk = numChunksPerThread * i;
        int lastChunk = (i == numThreads - 1) ? dataChunks.size()
                                              : firstChunk + numChunksPerThread;

        for (int k{firstChunk}; k < lastChunk; ++k) {
          for (int j{0}; j < dataChunks[k].size(); j++) {
            localFrequency[i][df->score[dataChunks[k][j]] - 1]++;
          }
        }

        localSum[i][0] = localFrequency[i][0];
        for (int k{1}; k < 5; k++) {
          localSum[i][k] = localSum[i][k - 1] + localFrequency[i][k];
        }
      });
    }
  }

  for (int i{0}; i < numThreads; ++i) {
    for (int j{0}; j < 5; j++) {
      cummulativeSum[j] += localSum[i][j];
    }
  }

  std::vector<int> countArray(df->rowCount);

  for (int i = df->rowCount - 1; i >= 0; --i) {
    int key = df->score[i] - 1;
    int index = cummulativeSum[key]--;
    countArray[index] = i;
  }

  return countArray;
}
} // namespace mth_css
