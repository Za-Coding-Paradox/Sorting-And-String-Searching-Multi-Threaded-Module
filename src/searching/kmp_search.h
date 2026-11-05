#pragma once
#include "../loader/chunker.h"
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace mth_css {

struct KMPSearchResult {
  std::vector<int> matchIndices;
  long long characterComparisons{0};
};

// Computes the Longest Proper Prefix which is also a Suffix (LPS) table
auto computeLPSArray(const char *pattern, int patternLen) -> std::vector<int> {
  std::vector<int> lps(patternLen, 0);
  int length{0};
  int i{1};

  while (i < patternLen) {
    if (pattern[i] == pattern[length]) {
      length++;
      lps[i] = length;
      i++;
    } else {
      if (length != 0) {
        length = lps[length - 1];
      } else {
        lps[i] = 0;
        i++;
      }
    }
  }

  return lps;
}

// Generic single-string KMP search using precomputed LPS table
// Counts each character comparison between text[i] and pattern[j] once
auto kmpSearchText(const char *text, int textLen, const char *pattern,
                   int patternLen, const std::vector<int> &lps)
    -> KMPSearchResult {
  KMPSearchResult result;
  if (!text || !pattern || patternLen <= 0 || textLen < patternLen) {
    return result;
  }

  int i{0}; // index for text
  int j{0}; // index for pattern

  while (i < textLen) {
    result.characterComparisons++;
    if (pattern[j] == text[i]) {
      i++;
      j++;

      if (j == patternLen) {
        result.matchIndices.push_back(i - j);
        j = lps[j - 1];
      }
    } else {
      if (j != 0) {
        j = lps[j - 1];
      } else {
        i++;
      }
    }
  }

  return result;
}

// Multi-threaded KMP search across DataFrame review texts
auto kmpSearch(DataFrame *df, const std::string &pattern,
               int maxReviews = 10000) -> KMPSearchResult {
  int totalReviews =
      (maxReviews > 0 && maxReviews < df->rowCount) ? maxReviews : df->rowCount;
  int numThreads = std::min(NUM_THREADS, std::max(1, totalReviews));
  int reviewsPerThread = totalReviews / numThreads;

  const char *patPtr = pattern.c_str();
  int patLen = (int)pattern.length();
  std::vector<int> lps = computeLPSArray(patPtr, patLen);

  std::vector<KMPSearchResult> localResults(numThreads);

  { // jthreads join automatically when this block ends
    std::vector<std::jthread> threadList(numThreads);

    for (int i{0}; i < numThreads; ++i) {
      threadList[i] = std::jthread([&, i]() {
        int startRow = i * reviewsPerThread;
        int endRow =
            (i == numThreads - 1) ? totalReviews : startRow + reviewsPerThread;

        for (int r{startRow}; r < endRow; ++r) {
          if (!df->text[r]) {
            continue;
          }
          int textLen = (int)std::strlen(df->text[r]);
          auto res = kmpSearchText(df->text[r], textLen, patPtr, patLen, lps);
          localResults[i].characterComparisons += res.characterComparisons;
          for (int idx : res.matchIndices) {
            localResults[i].matchIndices.push_back(r);
          }
        }
      });
    }
  }

  KMPSearchResult finalResult;
  for (int i{0}; i < numThreads; ++i) {
    finalResult.characterComparisons += localResults[i].characterComparisons;
    for (int rowIdx : localResults[i].matchIndices) {
      finalResult.matchIndices.push_back(rowIdx);
    }
  }

  return finalResult;
}

} // namespace mth_css
