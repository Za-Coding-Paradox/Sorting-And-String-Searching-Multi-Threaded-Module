#include "../loader/chunker.h"
#include <algorithm>
#include <iostream>
#include <vector>

namespace mth_css {
enum class HeapType { MIN, MAX };

// heap is stored as row indexes, ordered by the ratio column
auto heapify(std::vector<int> &heapArray, DataFrame *df, int i,
             HeapType typeOfHeap) -> void {
  if (i <= 0) {
    return;
  }

  int parent = (i - 1) / 2;

  if (typeOfHeap == HeapType::MIN &&
      df->ratio[heapArray[parent]] > df->ratio[heapArray[i]]) {
    std::swap(heapArray[parent], heapArray[i]);
  } else if (typeOfHeap == HeapType::MAX &&
             df->ratio[heapArray[parent]] < df->ratio[heapArray[i]]) {
    std::swap(heapArray[parent], heapArray[i]);
  }

  heapify(heapArray, df, parent, typeOfHeap);

  return;
}

auto buildHeap(HeapType typeOfHeap, DataFrame *df, const auto &dataChunks)
    -> std::vector<int> {
  std::vector<int> heapArray;

  for (int i{0}; i < dataChunks.size(); ++i) {
    for (int j{0}; j < dataChunks[i].size(); ++j) {
      heapArray.push_back(dataChunks[i][j]);
      heapify(heapArray, df, heapArray.size() - 1,
              typeOfHeap); // chunks may differ in size, so index by the
                           // heap's own size
    }
  }

  return heapArray;
}
} // namespace mth_css
