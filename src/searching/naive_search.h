#pragma once
#include "../loader/chunker.h"
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace mth_css {

struct NaiveSearchResult {
  std::vector<int> matchIndices;
  long long characterComparisons{0};
};

// Generic single-string Naive search returning match positions and comparison
// count
auto naiveSearchText(const char *text, int textLen, const char *pattern,
                     int patternLen) -> NaiveSearchResult {
  NaiveSearchResult result;
  if (!text || !pattern || patternLen <= 0 || textLen < patternLen) {
    return result;
  }

  for (int i{0}; i <= textLen - patternLen; ++i) {
    int j{0};
    for (; j < patternLen; ++j) {
      result.characterComparisons++;
      if (text[i + j] != pattern[j]) {
        break;
      }
    }
    if (j == patternLen) {
      result.matchIndices.push_back(i);
    }
  }

  return result;
}

// Multi-threaded Naive search across a collection of review strings
auto naiveSearch(const std::vector<std::string> &reviews,
                 const std::string &pattern, int maxReviews = -1)
    -> NaiveSearchResult {
  int totalReviews = (maxReviews > 0 && maxReviews < (int)reviews.size())
                         ? maxReviews
                         : (int)reviews.size();
  int numThreads = std::min(NUM_THREADS, std::max(1, totalReviews));
  int reviewsPerThread = totalReviews / numThreads;

  std::vector<NaiveSearchResult> localResults(numThreads);

  { // jthreads join automatically when this block ends
    std::vector<std::jthread> threadList(numThreads);

    for (int i{0}; i < numThreads; ++i) {
      threadList[i] = std::jthread([&, i]() {
        int startRow = i * reviewsPerThread;
        int endRow =
            (i == numThreads - 1) ? totalReviews : startRow + reviewsPerThread;

        for (int r{startRow}; r < endRow; ++r) {
          auto res =
              naiveSearchText(reviews[r].c_str(), (int)reviews[r].length(),
                              pattern.c_str(), (int)pattern.length());
          localResults[i].characterComparisons += res.characterComparisons;
          for (int idx : res.matchIndices) {
            localResults[i].matchIndices.push_back(r);
          }
        }
      });
    }
  }

  NaiveSearchResult finalResult;
  for (int i{0}; i < numThreads; ++i) {
    finalResult.characterComparisons += localResults[i].characterComparisons;
    for (int rowIdx : localResults[i].matchIndices) {
      finalResult.matchIndices.push_back(rowIdx);
    }
  }

  return finalResult;
}

} // namespace mth_css
