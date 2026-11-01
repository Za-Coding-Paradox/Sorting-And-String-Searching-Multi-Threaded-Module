#include "../loader/chunker.h"
#include <algorithm>
#include <iostream>
#include <vector>

namespace mth_css {
enum class HeapType { MIN, MAX };

auto buildHeap(HeapType typeOfHeap, auto dataChunks)
    -> std::vector<DataFrame *> {
  std::vector<DataFrame *> heapArray.reserve(numberOfChunks * sizeOfChunk);

  for (int i{0}; i < dataChunks.size(); ++i) {
    for (int j{0}; j < dataChunk[i].lenght(); ++j) {
      heapArray[j + i * dataChunks[0].length()] = dataChunks[i][j];
      heapify(heapArray, j + i * dataChunks[0].length(),
              typeOfHeap); // j* i because we don't know at
                           // which chunk we are at.
    }
  }

  return heapArray;
}

auto heapify(std::vector<DataFrame *> heapArray, int i, HeapType typeOfHeap) {
  if (i < 0) {
    return;
  }

  int parent = i / 2;

  if (parent >= 0) {
    if (typeOfHeap == HeapType::MIN && heapArray[parent] > hepaArray[i]) {
      swap(heapArray[parent], heapArray[i]);
    } else if (typeOfHeap == HeapType::MAX &&
               heapArray[parent] < heapArray[i]) {
      swap(heapArray[parent], heapArray[i]);
    }
  }

  heapify(heapArray, parent, typeOfHeap);

  return;
}
} // namespace mth_css
