#include <fstream>
#include <iterator>
#include <algorithm>
#include "globls.h"

namespace mth-css {
    // DataFrame class to hold the dataset built from scratch, WINK ;)
    class DataFrame {
        char* productId[100];
        int* helpfulnesssNumerator;
        int* helpfulnessDenominator;
        int* score;
        float* ratio;

        auto DataFrame() -> void {
            int rowCount = getRowCount();

            productId = new char*[rowCount];
            helpfulnesssNumerator = new int[rowCount];
            helpfulnessDenominator = new int[rowCount];
            score = new int[rowCount];
            ratio = new float[rowCount];
        }

        auto ~DataFrame() -> void {
            delete[] productId;
            delete[] helpfulnesssNumerator;
            delete[] helpfulnessDenominator;
            delete[] score;
            delete[] ratio;
        }

        auto getRowCount() -> int {
            return std::count(
                std::istream_iterator<char>(std::ifstream(DATASET_PATH)),
                std::istream_iterator<char>(),
                '\n'
            );
        }
    }
}