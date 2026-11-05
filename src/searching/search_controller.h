#pragma once
#include "kmp_search.h"
#include "naive_search.h"
#include "rabin_karp.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace mth_css {

class SearchingController {
private:
  DataFrame *df;
  std::string concatenatedBuffer;

  auto buildCharacterBuffer(int charLimit = 100000) -> void {
    concatenatedBuffer.clear();
    if (!df) {
      return;
    }
    concatenatedBuffer.reserve(charLimit + 1024);
    for (int i{0}; i < df->rowCount; ++i) {
      if (df->text[i]) {
        concatenatedBuffer += df->text[i];
        concatenatedBuffer += ' ';
        if ((int)concatenatedBuffer.length() >= charLimit) {
          break;
        }
      }
    }
    if ((int)concatenatedBuffer.length() > charLimit) {
      concatenatedBuffer.resize(charLimit);
    }
  }

public:
  SearchingController(DataFrame *dataFrame, int charLimit = 100000)
      : df(dataFrame) {
    buildCharacterBuffer(charLimit);
  }

  // Task 3.1: Compare Multi-Threaded Naive Search and KMP across first 10,000
  // reviews
  auto runTask3_1(const std::string &phrase = "highly recommend",
                  int numReviews = 10000) -> void {
    std::cout << "\n=======================================================\n";
    std::cout << "Task 3.1: Naive Search vs. KMP (Multi-Threaded)\n";
    std::cout << "Phrase: \"" << phrase << "\" | Reviews: " << numReviews
              << "\n";
    std::cout << "=======================================================\n";

    auto startNaive = std::chrono::high_resolution_clock::now();
    auto naiveRes = naiveSearch(df, phrase, numReviews);
    auto endNaive = std::chrono::high_resolution_clock::now();
    double naiveMs =
        std::chrono::duration<double, std::milli>(endNaive - startNaive)
            .count();

    auto startKMP = std::chrono::high_resolution_clock::now();
    auto kmpRes = kmpSearch(df, phrase, numReviews);
    auto endKMP = std::chrono::high_resolution_clock::now();
    double kmpMs =
        std::chrono::duration<double, std::milli>(endKMP - startKMP).count();

    std::cout << std::left << std::setw(22) << "Metric" << std::setw(20)
              << "Naive Search" << std::setw(20) << "KMP Search" << "\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << std::left << std::setw(22) << "Matches Found" << std::setw(20)
              << naiveRes.matchIndices.size() << std::setw(20)
              << kmpRes.matchIndices.size() << "\n";
    std::cout << std::left << std::setw(22) << "Char Comparisons"
              << std::setw(20) << naiveRes.characterComparisons << std::setw(20)
              << kmpRes.characterComparisons << "\n";
    std::cout << std::left << std::setw(22) << "Execution Time (ms)"
              << std::setw(20) << naiveMs << std::setw(20) << kmpMs << "\n";

    // Compute and display LPS table for "really really good"
    std::string lpsTarget = "really really good";
    auto lps = computeLPSArray(lpsTarget.c_str(), (int)lpsTarget.length());
    std::cout << "\nLPS Array for \"" << lpsTarget << "\":\n[ ";
    for (int val : lps) {
      std::cout << val << " ";
    }
    std::cout << "]\n";
  }

  // Task 3.2: Multi-Threaded Rabin-Karp with small and large prime moduli on
  // first 100,000 chars
  auto runTask3_2(const std::string &phrase = "gluten free",
                  int charLimit = 100000) -> void {
    if ((int)concatenatedBuffer.length() < charLimit) {
      buildCharacterBuffer(charLimit);
    }

    std::cout << "\n=======================================================\n";
    std::cout << "Task 3.2: Rabin-Karp and Spurious Hits (Multi-Threaded)\n";
    std::cout << "Phrase: \"" << phrase << "\" | First " << charLimit
              << " characters\n";
    std::cout << "=======================================================\n";

    long long smallPrime = 101;
    long long largePrime = 1000000007LL;

    auto startSmall = std::chrono::high_resolution_clock::now();
    auto resSmall =
        rabinKarpSearch(concatenatedBuffer, phrase, smallPrime, charLimit);
    auto endSmall = std::chrono::high_resolution_clock::now();
    double smallMs =
        std::chrono::duration<double, std::milli>(endSmall - startSmall)
            .count();

    auto startLarge = std::chrono::high_resolution_clock::now();
    auto resLarge =
        rabinKarpSearch(concatenatedBuffer, phrase, largePrime, charLimit);
    auto endLarge = std::chrono::high_resolution_clock::now();
    double largeMs =
        std::chrono::duration<double, std::milli>(endLarge - startLarge)
            .count();

    std::cout << std::left << std::setw(22) << "Metric" << std::setw(20)
              << "q = 101" << std::setw(20) << "q = 1,000,000,007" << "\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << std::left << std::setw(22) << "Hash Hits" << std::setw(20)
              << resSmall.hashHits << std::setw(20) << resLarge.hashHits
              << "\n";
    std::cout << std::left << std::setw(22) << "Verified Matches"
              << std::setw(20) << resSmall.verifiedMatches << std::setw(20)
              << resLarge.verifiedMatches << "\n";
    std::cout << std::left << std::setw(22) << "Spurious Hits" << std::setw(20)
              << resSmall.spuriousHits << std::setw(20) << resLarge.spuriousHits
              << "\n";
    std::cout << std::left << std::setw(22) << "Execution Time (ms)"
              << std::setw(20) << smallMs << std::setw(20) << largeMs << "\n";
  }
};

} // namespace mth_css
