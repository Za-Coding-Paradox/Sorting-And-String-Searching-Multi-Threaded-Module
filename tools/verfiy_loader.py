import sys
import pandas as pd

# Load the dataset and perform basic cleaning and filtering
# the system accepts a path to the dataset as a command line argument, otherwise it defaults to "dataset/Reviews.csv"
path = sys.argv[1] if len(sys.argv) > 1 else "dataset/Reviews.csv"
df = pd.read_csv(path)
print("raw rows:", len(df))

# Drop rows with missing values in critical columns and drop unnecessary columns
df = df.dropna(subset=["ProductId", "Score", "Time",
                       "HelpfulnessNumerator", "HelpfulnessDenominator", "Text"])
df = df.dropcolumns(["Id", "UserId", "ProfileName", "Summary"])
print("after dropna:", len(df))

df = df[df.HelpfulnessDenominator > 0]
df = df[df.HelpfulnessNumerator / df.HelpfulnessDenominator <= 1]
print("after ratio filter:", len(df))

print("score sum:", int(df.Score.sum()))
print("time sum:", int(df.Time.sum()))
print("den sum:", int(df.HelpfulnessDenominator.sum()))

clean = df.Text.str.replace(r"<[^>]*>", " ", regex=True).str.lower()
for x in clean.head(3):
    print(repr(x[:100]))