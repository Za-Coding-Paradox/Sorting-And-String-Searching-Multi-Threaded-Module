#pragma once
#include "bucket_sort.h"
#include "count_sort.h"
#include "heap_sort.h"
#include "radix_sort.h"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

namespace mth_css {

class SortingController {
private:
  DataFrame *df;
  std::vector<std::vector<int>> dataChunks;

  // 0-based sift-down for in-place Heap Sort extraction
  auto siftDown(std::vector<int> &heapArray, int n, int i, HeapType typeOfHeap)
      -> void {
    int extreme = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (typeOfHeap == HeapType::MIN) {
      if (left < n && df->helpfulnessDenominator[heapArray[left]] <
                          df->helpfulnessDenominator[heapArray[extreme]]) {
        extreme = left;
      }
      if (right < n && df->helpfulnessDenominator[heapArray[right]] <
                           df->helpfulnessDenominator[heapArray[extreme]]) {
        extreme = right;
      }
    } else {
      if (left < n && df->helpfulnessDenominator[heapArray[left]] >
                          df->helpfulnessDenominator[heapArray[extreme]]) {
        extreme = left;
      }
      if (right < n && df->helpfulnessDenominator[heapArray[right]] >
                           df->helpfulnessDenominator[heapArray[extreme]]) {
        extreme = right;
      }
    }

    if (extreme != i) {
      std::swap(heapArray[i], heapArray[extreme]);
      siftDown(heapArray, n, extreme, typeOfHeap);
    }
  }

public:
  SortingController(DataFrame *dataFrame,
                    const std::vector<std::vector<int>> &chunks)
      : df(dataFrame), dataChunks(chunks) {}

  // Task 1.1: Heap Sort and Top-K (Descending order using a Min-Heap)
  auto runTask1_1(int topK = 10) -> std::vector<int> {
    std::cout << "\n=======================================================\n";
    std::cout << "Task 1.1: Heap Sort and Top-" << topK
              << " (Min-Heap Descending)\n";
    std::cout << "=======================================================\n";

    auto start = std::chrono::high_resolution_clock::now();

    // Build 0-based Min-Heap usingsrc/sorting/heap_sort.h
    std::vector<int> heapArray = buildHeap(HeapType::MIN, df, dataChunks);

    // Extract minimum repeatedly to sort in descending order in-place
    for (int i = (int)heapArray.size() - 1; i > 0; --i) {
      std::swap(heapArray[0], heapArray[i]);
      siftDown(heapArray, i, 0, HeapType::MIN);
    }

    auto end = std::chrono::high_resolution_clock::now();
    double elapsedMs =
        std::chrono::duration<double, std::milli>(end - start).count();

    int limit = std::min(topK, (int)heapArray.size());
    std::cout << std::left << std::setw(10) << "Rank" << std::setw(16)
              << "Row Index" << std::setw(20) << "ProductId" << std::setw(24)
              << "HelpfulnessDenominator" << "\n";
    std::cout << "-------------------------------------------------------\n";
    for (int i{0}; i < limit; ++i) {
      int rowIdx = heapArray[i];
      std::cout << std::left << std::setw(10) << (i + 1) << std::setw(16)
                << rowIdx << std::setw(20)
                << (df->productId[rowIdx] ? df->productId[rowIdx] : "N/A")
                << std::setw(24) << df->helpfulnessDenominator[rowIdx] << "\n";
    }
    std::cout << "Execution Time: " << elapsedMs << " ms\n";

    return heapArray;
  }

  // Task 2.1: Counting Sort on Product Scores (1 through 5)
  auto runTask2_1() -> std::vector<int> {
    std::cout << "\n=======================================================\n";
    std::cout << "Task 2.1: Counting Sort and Product Scores (1 to 5)\n";
    std::cout << "=======================================================\n";

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> sortedIndices = countSort(df, dataChunks);
    auto end = std::chrono::high_resolution_clock::now();
    double elapsedMs =
        std::chrono::duration<double, std::milli>(end - start).count();

    int scoreCounts[5] = {0};
    for (int i{0}; i < df->rowCount; ++i) {
      int s = df->score[i];
      if (s >= 1 && s <= 5) {
        scoreCounts[s - 1]++;
      }
    }

    std::cout << std::left << std::setw(15) << "Score" << std::setw(20)
              << "Count" << "\n";
    std::cout << "-----------------------------------\n";
    for (int s{0}; s < 5; ++s) {
      std::cout << std::left << std::setw(15) << (s + 1) << std::setw(20)
                << scoreCounts[s] << "\n";
    }
    std::cout << "Execution Time: " << elapsedMs << " ms\n";

    return sortedIndices;
  }

  // Task 2.2: LSD Radix Sort on Timestamps (Base-10 vs. Base-256)
  auto runTask2_2() -> void {
    std::cout << "\n=======================================================\n";
    std::cout
        << "Task 2.2: LSD Radix Sort on Timestamps (Base-10 vs Base-256)\n";
    std::cout << "=======================================================\n";

    auto start10 = std::chrono::high_resolution_clock::now();
    std::vector<int> sortedBase10 = radixSort(10, df);
    auto end10 = std::chrono::high_resolution_clock::now();
    double base10Ms =
        std::chrono::duration<double, std::milli>(end10 - start10).count();

    auto start256 = std::chrono::high_resolution_clock::now();
    std::vector<int> sortedBase256 = radixSort(256, df);
    auto end256 = std::chrono::high_resolution_clock::now();
    double base256Ms =
        std::chrono::duration<double, std::milli>(end256 - start256).count();

    std::cout << std::left << std::setw(22) << "Radix Base" << std::setw(22)
              << "Execution Time (ms)" << std::setw(15) << "Rows Sorted"
              << "\n";
    std::cout << "-------------------------------------------------------\n";
    std::cout << std::left << std::setw(22) << "Base-10 (Digits)"
              << std::setw(22) << base10Ms << std::setw(15)
              << sortedBase10.size() << "\n";
    std::cout << std::left << std::setw(22) << "Base-256 (Bytes)"
              << std::setw(22) << base256Ms << std::setw(15)
              << sortedBase256.size() << "\n";
  }

  // Task 2.3: Bucket Sort on Helpfulness Ratio
  auto runTask2_3() -> std::vector<int> {
    std::cout << "\n=======================================================\n";
    std::cout << "Task 2.3: Bucket Sort on Helpfulness Ratio\n";
    std::cout << "=======================================================\n";

    // Compute and report bucket sizes matching bucket_sort.h distribution
    std::vector<int> bucketSizes(11, 0);
    for (int i{0}; i < df->rowCount; ++i) {
      float value = df->ratio[i];
      bucketSizes[(int)std::ceil(value * 10)]++;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> sortedIndices = bucket_sort(df, dataChunks);
    auto end = std::chrono::high_resolution_clock::now();
    double elapsedMs =
        std::chrono::duration<double, std::milli>(end - start).count();

    std::cout << std::left << std::setw(15) << "Bucket Index" << std::setw(22)
              << "Ratio Range" << std::setw(18) << "Bucket Size" << "\n";
    std::cout << "-------------------------------------------------------\n";
    for (int b{0}; b < (int)bucketSizes.size(); ++b) {
      std::string rangeStr = (b == 0)
                                 ? "[0.0]"
                                 : "(" + std::to_string((b - 1) * 10) + "%, " +
                                       std::to_string(b * 10) + "%]";
      std::cout << std::left << std::setw(15) << b << std::setw(22) << rangeStr
                << std::setw(18) << bucketSizes[b] << "\n";
    }
    std::cout << "Execution Time: " << elapsedMs << " ms\n";

    return sortedIndices;
  }
};

} // namespace mth_css
