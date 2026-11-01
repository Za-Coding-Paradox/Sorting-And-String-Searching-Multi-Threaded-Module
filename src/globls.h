#pragma once
#include <algorithm>
#include <filesystem>
#include <thread>

// Defining path to the dataset file using std::filesystem for better path handling
// (the loader expects the cleaned 5-column file written by tools/preprocess.py)
namespace fs = std::filesystem;
const fs::path DATASET_PATH = fs::path("../dataset/Reviews_clean.csv");

// Defining the number of threads to be used for parallel processing, with a cap at 4 threads to avoid excessive context switching and resource contention.
// hardware_concurrency() may return 0 when unknown, so we never go below 1.
const int NUM_THREADS = std::max(1, std::min(4, (int)std::thread::hardware_concurrency()));
// This function returns the number of concurrent threads supported by the implementation.
// We use std::min to ensure we don't exceed 4 threads, which is a reasonable limit for most systems to avoid excessive context switching and resource contention.
