#pragma once
#include "../loader/chunker.h"
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace mth_css {

struct RabinKarpResult {
  std::vector<int> matchIndices;
  long long hashHits{0};
  long long verifiedMatches{0};
  long long spuriousHits{0};
};

// Generic single-segment Rabin-Karp string search using a rolling polynomial
// hash
auto rabinKarpSearchText(const char *text, int textLen, const char *pattern,
                         int patternLen, long long primeModulus,
                         long long base = 256, int globalOffset = 0)
    -> RabinKarpResult {
  RabinKarpResult result;
  if (!text || !pattern || patternLen <= 0 || textLen < patternLen) {
    return result;
  }

  long long h{1};
  for (int i{0}; i < patternLen - 1; ++i) {
    h = (h * base) % primeModulus;
  }

  long long patternHash{0};
  long long windowHash{0};

  // Initial hash calculation for pattern and first window
  for (int i{0}; i < patternLen; ++i) {
    patternHash =
        (base * patternHash + (unsigned char)pattern[i]) % primeModulus;
    windowHash = (base * windowHash + (unsigned char)text[i]) % primeModulus;
  }

  for (int i{0}; i <= textLen - patternLen; ++i) {
    if (patternHash == windowHash) {
      result.hashHits++;
      bool match{true};
      for (int j{0}; j < patternLen; ++j) {
        if (text[i + j] != pattern[j]) {
          match = false;
          break;
        }
      }

      if (match) {
        result.verifiedMatches++;
        result.matchIndices.push_back(globalOffset + i);
      } else {
        result.spuriousHits++;
      }
    }

    // Compute rolling hash for next window
    if (i < textLen - patternLen) {
      long long leadingVal = ((unsigned char)text[i] * h) % primeModulus;
      windowHash = (windowHash - leadingVal + primeModulus) % primeModulus;
      windowHash = (windowHash * base + (unsigned char)text[i + patternLen]) %
                   primeModulus;
    }
  }

  return result;
}

// Multi-threaded Rabin-Karp string search across a continuous character buffer
// Uses overlapping slices of (patternLen - 1) characters so boundary matches
// are never missed
auto rabinKarpSearch(const std::string &buffer, const std::string &pattern,
                     long long primeModulus, int maxChars = 100000,
                     long long base = 256) -> RabinKarpResult {
  RabinKarpResult finalResult;
  int totalLen = (maxChars > 0 && maxChars < (int)buffer.length())
                     ? maxChars
                     : (int)buffer.length();
  int patLen = (int)pattern.length();

  if (patLen <= 0 || totalLen < patLen) {
    return finalResult;
  }

  int totalWindows = totalLen - patLen + 1;
  int numThreads = std::min(NUM_THREADS, std::max(1, totalWindows));
  int windowsPerThread = totalWindows / numThreads;

  std::vector<RabinKarpResult> localResults(numThreads);

  { // jthreads join automatically when this block ends
    std::vector<std::jthread> threadList(numThreads);

    for (int i{0}; i < numThreads; ++i) {
      threadList[i] = std::jthread([&, i]() {
        int startWindow = i * windowsPerThread;
        int endWindow = (i == numThreads - 1) ? totalWindows
                                              : startWindow + windowsPerThread;
        int numWindows = endWindow - startWindow;
        int sliceLen = numWindows + patLen - 1;

        localResults[i] = rabinKarpSearchText(buffer.c_str() + startWindow,
                                              sliceLen, pattern.c_str(), patLen,
                                              primeModulus, base, startWindow);
      });
    }
  }

  for (int i{0}; i < numThreads; ++i) {
    finalResult.hashHits += localResults[i].hashHits;
    finalResult.verifiedMatches += localResults[i].verifiedMatches;
    finalResult.spuriousHits += localResults[i].spuriousHits;
    for (int idx : localResults[i].matchIndices) {
      finalResult.matchIndices.push_back(idx);
    }
  }

  return finalResult;
}

} // namespace mth_css
