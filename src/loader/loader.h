#include "../dataframe/dataframe.h"

namespace mth-css {
    class Loader {
        DataFrame* dataFrame;

        auto Loader() -> void {
            dataFrame = new DataFrame();
        }

        auto getDataFrame() -> DataFrame* {
            return dataFrame;
        }

        auto loadData() -> void {
            fstream dataset = std::ifstream(DATASET_PATH);
            
            // Read the dataset and populate the DataFrame object
            const int rowCount = dataFrame->getRowCount();

            // Read the first word/cell normally (up to the comma)
            std::string firstCell;
            std::getline(file, firstCell, ',');
            std::cout << "Read first cell: " << firstCell << std::endl;

            // This line will read the rest of the line and discard it, effectively skipping to the next line
            std::string dummy;
            std::getline(file, dummy); // Reads everything remaining on this line and drops it
            

            // Running till rowCount - 1 because the firest line will be skipped as it contains the column names
            for (int i = 0; i < rowCount - 1; ++i) {
                std::string productId;
                int helpfulnessNumerator;
                int helpfulnessDenominator;
                int score;
                float ratio;

                // Read the first cell (productId)
                // This reads the chacters up to the next comma and stores them in the productId string. So we need to type cast the string into a char array.
                std::getline(dataset, productId, ',');
                dataFrame->productId[i] = new char [productId.length() + 1]; // Allocate memory for the char array
                std::strcpy(dataFrame->productId[i], productId.c_str());

                // Read the next cells (helpfulnessNumerator, helpfulnessDenominator, score, ratio)
                dataset >> dataFrame->helpfulnesssNumerator[i];
                dataset.ignore(); // Ignore the comma
                dataset >> dataFrame->helpfulnessDenominator[i];
                dataset.ignore(); // Ignore the comma
                dataset >> dataFrame->score[i];
                dataset.ignore(); // Ignore the comma
                dataset >> dataFrame->ratio[i];

                // Ignore the rest of the line to move to the next row
                dataset.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }

            dataset.close();
        }

        auto ~Loader() -> void {
            delete dataFrame;
        }
    }
}