import subprocess
import sys
from pathlib import Path


def main():
    print("=== Step 1: Preprocessing Dataset (Python Workflow) ===")
    raw_csv = Path("dataset/Reviews.csv")
    clean_csv = Path("dataset/Reviews_clean.csv")

    if not clean_csv.exists():
        if raw_csv.exists():
            print(f"Running tools/preprocess.py on {raw_csv}...")
            subprocess.run(
                [sys.executable, "tools/preprocess.py", str(raw_csv), str(clean_csv)],
                check=True,
            )
        else:
            print(
                f"Warning: {raw_csv} not found. Please place Reviews.csv in the dataset/ directory."
            )
    else:
        print(f"Found clean dataset at {clean_csv}.")

    print("\n=== Step 2: Configuring CMake & Building C++ Binary ===")
    build_dir = Path("build")
    build_dir.mkdir(exist_ok=True)

    subprocess.run(["cmake", ".."], cwd=build_dir, check=True)
    subprocess.run(
        ["cmake", "--build", ".", "--config", "Release"], cwd=build_dir, check=True
    )

    print("\n=== Build Complete ===")
    print("You can run the application from the build directory or via Python.")


if __name__ == "__main__":
    main()
