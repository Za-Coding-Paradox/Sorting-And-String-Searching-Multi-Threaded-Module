#pragma once
#include "../dataframe/dataframe.h"
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace mth_css {
class Loader {
  DataFrame *dataFrame;

public:
  Loader() { dataFrame = new DataFrame(); }

  auto getDataFrame() -> DataFrame * { return dataFrame; }

  auto loadData() -> void {
    // Read the dataset and populate the DataFrame object
    const int rowCount = dataFrame->rowCount;

    // Create the list of threads
    std::vector<std::jthread> threadList(NUM_THREADS);

    // Launch threads to read the dataset in parallel
    for (int i{0}; i < NUM_THREADS; ++i) {

      // std::thread takes an function as parameter to execute. The function may
      // be a wrapper, or even a lamda.
      threadList[i] =
          std::jthread( // Waits for all thread in threadlist to finish and
                        // joins them automatically Standard thread would
                        // require a join loop after the current function loop
              [this, i,
               rowCount]() { // Creates a lamda function with thread-index
                             // and rowCount as arguments to the load the
                             // dataset in parallel across multiple ranges
                std::ifstream dataset(DATASET_PATH);

                if (!dataset.is_open()) {
                  std::cerr << "Error opening file: " << DATASET_PATH
                            << std::endl;
                  return;
                }

                // Skip the first line (header)
                std::string headerLine;
                std::getline(dataset, headerLine);

                // Calculate the number of rows each thread should process
                int rowsPerThread = rowCount / NUM_THREADS;
                int startRow = i * rowsPerThread;
                int endRow = (i == NUM_THREADS - 1) ? rowCount
                                                    : startRow + rowsPerThread;

                // Move the file pointer to the start of the assigned rows
                for (int j = 0; j < startRow; ++j) {
                  dataset.ignore(std::numeric_limits<std::streamsize>::max(),
                                 '\n');
                  // Stop ignoring at infinity count, or at first '\n'
                }

                for (int j = startRow; j < endRow; ++j) {
                  std::string productId;
                  int helpfulnessNumerator;
                  int helpfulnessDenominator;
                  int score;
                  float ratio;

                  // Read the first cell (productId)
                  std::getline(dataset, productId, ',');
                  dataFrame->productId[j] =
                      new char[productId.length() +
                               1]; // Allocate memory for the char array
                  std::strcpy(dataFrame->productId[j], productId.c_str());

                  // Read the next cells (helpfulnessNumerator,
                  // helpfulnessDenominator, score, ratio)
                  dataset >> dataFrame->helpfulnesssNumerator[j];
                  dataset.ignore(); // Ignore the comma
                  dataset >> dataFrame->helpfulnessDenominator[j];
                  dataset.ignore(); // Ignore the comma
                  dataset >> dataFrame->score[j];
                  dataset.ignore(); // Ignore the comma
                  dataset >> dataFrame->ratio[j];

                  // Ignore the rest of the line to move to the next row
                  dataset.ignore(std::numeric_limits<std::streamsize>::max(),
                                 '\n');
                }

                dataset.close();
              });
    }
  }

  ~Loader() { delete dataFrame; }
};
} // namespace mth_css
