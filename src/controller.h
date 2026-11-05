#pragma once
#include "dataframe/dataframe.h"
#include "loader/chunker.h"
#include "searching/pipeline_controller.h"
#include "searching/searching_controller.h"
#include "sorting/sorting_controller.h"
#include <iostream>
#include <vector>

namespace mth_css {

class Controller {
private:
  Chunker chunker;
  DataFrame *df;
  std::vector<std::vector<int>> dataChunks;

  SortingController sortingCtrl;
  SearchingController searchingCtrl;
  PipelineController pipelineCtrl;

public:
  Controller(int numChunks = 4, int chunkSize = 25000)
      : chunker(numChunks, chunkSize), df(nullptr), sortingCtrl(nullptr, {}),
        searchingCtrl(nullptr), pipelineCtrl(nullptr) {}

  auto initialize() -> bool {
    std::cout << "=======================================================\n";
    std::cout << " DAA Assignment 3: Sorting & String Searching Module   \n";
    std::cout << "=======================================================\n";
    std::cout << "Loading dataset from " << DATASET_PATH << " ...\n";

    chunker.createChunks();
    df = chunker.getDataFrame();
    dataChunks = chunker.getDataChunks();

    if (!df || df->rowCount <= 0) {
      std::cerr << "Error: Dataset failed to load or is empty.\n";
      return false;
    }

    std::cout << "Dataset loaded successfully. Total rows: " << df->rowCount
              << "\n";

    sortingCtrl = SortingController(df, dataChunks);
    searchingCtrl = SearchingController(df);
    pipelineCtrl = PipelineController(df);

    return true;
  }

  auto runMenu() -> void {
    int choice{0};
    do {
      std::cout << "\n==================== MAIN MENU ====================\n";
      std::cout
          << "1. Run Part 1: Heap Sort & Top-10 Denominators (Task 1.1)\n";
      std::cout << "2. Run Part 2.1: Counting Sort on Product Scores (1-5)\n";
      std::cout
          << "3. Run Part 2.2: Radix Sort on Timestamps (Base-10 vs 256)\n";
      std::cout << "4. Run Part 2.3: Bucket Sort on Helpfulness Ratio\n";
      std::cout
          << "5. Run Part 3.1: Naive Search vs. KMP ('highly recommend')\n";
      std::cout
          << "6. Run Part 3.2: Rabin-Karp & Spurious Hits ('gluten free')\n";
      std::cout << "7. Run Part 4.1: The 'Gluten Free' Mini-Pipeline (Top "
                   "Products)\n";
      std::cout << "8. Run All Tasks\n";
      std::cout << "0. Exit\n";
      std::cout << "Enter your choice: ";

      if (!(std::cin >> choice)) {
        break;
      }

      switch (choice) {
      case 1:
        sortingCtrl.runTask1_1(10);
        break;
      case 2:
        sortingCtrl.runTask2_1();
        break;
      case 3:
        sortingCtrl.runTask2_2();
        break;
      case 4:
        sortingCtrl.runTask2_3();
        break;
      case 5:
        searchingCtrl.runTask3_1("highly recommend", 10000);
        break;
      case 6:
        searchingCtrl.runTask3_2("gluten free", 100000);
        break;
      case 7:
        pipelineCtrl.runTask4_1(50, 10);
        break;
      case 8:
        sortingCtrl.runTask1_1(10);
        sortingCtrl.runTask2_1();
        sortingCtrl.runTask2_2();
        sortingCtrl.runTask2_3();
        searchingCtrl.runTask3_1("highly recommend", 10000);
        searchingCtrl.runTask3_2("gluten free", 100000);
        pipelineCtrl.runTask4_1(50, 10);
        break;
      case 0:
        std::cout << "Exiting...\n";
        break;
      default:
        std::cout << "Invalid choice. Try again.\n";
      }
    } while (choice != 0);
  }
};

} // namespace mth_css
