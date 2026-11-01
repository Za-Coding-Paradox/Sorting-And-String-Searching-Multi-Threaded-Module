#pragma once
#include "../loader/loader.h"
#include <iostream>
#include <vector>

namespace mth_css {
class Chunker {
private:
  // each chunk holds row indexes into the DataFrame (DataFrame stores columns,
  // so a "row" is just its index)
  std::vector<std::vector<int>> dataChunks;
  int numberOfChunks;
  int sizeOfChunk;

  Loader sysLoader;

public:
  Chunker(int numChunks, int chunkSize)
      : numberOfChunks(numChunks), sizeOfChunk(chunkSize) {}

  auto getDataFrame() -> DataFrame * { return sysLoader.getDataFrame(); }

  auto createChunks() -> void {
    sysLoader.loadData(); // nothing else ever fills the DataFrame

    // Get the DataFrame pointer
    auto dataFrame = sysLoader.getDataFrame();
    int rowCount = dataFrame->rowCount;

    dataChunks.resize(numberOfChunks);
    for (int i{0}; i < numberOfChunks; ++i) {
      int start = sizeOfChunk * i;
      int end = std::min(start + sizeOfChunk, rowCount);
      for (int j = start; j < end; ++j) {
        dataChunks[i].push_back(
            j); // accessing two dimensional vector via 1D access methods
      }
    }
  }

  auto getDataChunks() -> std::vector<std::vector<int>> { return dataChunks; }
};
} // namespace mth_css
