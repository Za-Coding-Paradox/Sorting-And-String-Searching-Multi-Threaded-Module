#pragma once
#include "../globls.h"
#include <algorithm>
#include <fstream>
#include <iterator>

namespace mth_css {
// DataFrame class to hold the dataset built from scratch, WINK ;)
class DataFrame {
public:
  char **productId;
  int *helpfulnesssNumerator;
  int *helpfulnessDenominator;
  int *score;
  float *ratio;
  int rowCount;

  DataFrame() {
    rowCount = getRowCount();

    productId = new char *[rowCount]();
    helpfulnesssNumerator = new int[rowCount];
    helpfulnessDenominator = new int[rowCount];
    score = new int[rowCount];
    ratio = new float[rowCount];
  }

  ~DataFrame() {
    for (int i = 0; i < rowCount; ++i) {
      delete[] productId[i];
    }
    delete[] productId;
    delete[] helpfulnesssNumerator;
    delete[] helpfulnessDenominator;
    delete[] score;
    delete[] ratio;
  }

  auto getRowCount() -> int {
    std::ifstream file(DATASET_PATH);
    int lines = (int)std::count(std::istreambuf_iterator<char>(file),
                                std::istreambuf_iterator<char>(), '\n');
    // the first line is the header, not a data row
    return lines > 0 ? lines - 1 : 0;
  }
};
} // namespace mth_css
