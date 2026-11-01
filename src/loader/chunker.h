#include "../loader/loader.h"
#include <iostream>
#include <vector>

namespace mth_css {
class Chunker {
private:
  std::vector<std::vector<DataFrame>> dataChunks;
  int numberOfChunks;
  int sizeOfChunk;

  Loader sysLoader;

public:
  auto Chunker(int numChunks, int chunkSize) -> void
      : numberOfChunks(numChunks),
        sizeOfChunk(chunkSize) {}

  auto createChunks() -> void {
    // Get the DataFrame pointer
    auto dataFrame = sysLoader->getDataFrame();
    int rowCount = dataFrame->rowCount;
    int chunkSize = rowCount / sizeOfChunk;

    dataChunks.reserve(numberOfChunks);
    for (int i{0}; i < numberOfChunks; ++i) {
      dataChunks[i].reserve(sizeOfChunk);
      memcopy(&dataChunks[i], dataFrame + (sizeOfChunk * i),
              sizeOfChunk * sizeof(DataFrame));
    }
  }

  auto getDataChunks() -> std::vector<std::vector<DataFrame>> {
    return dataChunks;
  }
}
} // namespace mth_css
