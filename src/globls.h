#include <filesystem>
#include <thread>

// Defining path to the dataset file using std::filesystem for better path handling
namespace fs = std::filesystem;
const fs::path DATASET_PATH = fs::path("../dataset/Reviews.csv");

// Defining the number of threads to be used for parallel processing, with a cap at 4 threads to avoid excessive context switching and resource contention.
auto const NUM_THREADS -> int = std::min(4, std::thread::hardware_concurrency()); 
// This function returns the number of concurrent threads supported by the implementation. 
// We use std::min to ensure we don't exceed 4 threads, which is a reasonable limit for most systems to avoid excessive context switching and resource contention.