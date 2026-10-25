"""Raw Amazon Fine Food Reviews.csv  ->  the 5-column CSV that src/loader/loader.h expects.

Why this exists: the raw file has 10 columns and quoted Text fields containing commas and newlines.
A line-based, comma-splitting C++ loader cannot parse that safely, so pandas (a real CSV parser)
does the cleaning once, and the C++ side reads a plain, trivially splittable file.

Writes two files:
  <out>.csv       ProductId,HelpfulnessNumerator,HelpfulnessDenominator,Score,Ratio
  <out>.expected  key=value reference values the C++ binary checks itself against (`make verify`)

usage: python tools/preprocess.py [raw.csv] [clean.csv]
"""
import sys
from pathlib import Path

import pandas as pd

REQUIRED = ["ProductId", "Score", "Time", "HelpfulnessNumerator", "HelpfulnessDenominator", "Text"]
DROP = ["Id", "UserId", "ProfileName", "Summary"]


def clean(df: pd.DataFrame):
    """Single source of truth for cleaning; tools/verfiy_loader.py imports this too, so they cannot drift.
    Returns (cleaned_df, stage_counts)."""
    stages = {"raw rows": len(df)}
    df = df.dropna(subset=REQUIRED)
    stages["after dropna"] = len(df)
    df = df.drop(columns=DROP, errors="ignore")
    df = df[df.HelpfulnessDenominator > 0]
    df = df[df.HelpfulnessNumerator / df.HelpfulnessDenominator <= 1]
    stages["after ratio filter"] = len(df)
    return df, stages


def id_hash(ids) -> int:
    """64-bit FNV-1a over each id followed by '\\n', in row order. Must match DataFrame::summarize()."""
    h, mask = 1469598103934665603, (1 << 64) - 1
    for s in ids:
        for b in (s + "\n").encode():
            h = ((h ^ b) * 1099511628211) & mask
    return h


def main() -> int:
    raw = sys.argv[1] if len(sys.argv) > 1 else "dataset/Reviews.csv"
    out = Path(sys.argv[2] if len(sys.argv) > 2 else "dataset/Reviews_clean.csv")

    df, stages = clean(pd.read_csv(raw))
    for k, v in stages.items():
        print(f"{k}: {v}")

    # The C++ loader splits on ',' with no quote handling: refuse to emit anything it would misparse.
    bad = df.ProductId.astype(str).str.contains(r'[,"\r\n]', regex=True)
    if bad.any():
        sys.exit(f"{int(bad.sum())} ProductId values contain ',', '\"' or a newline; loader cannot handle them")

    num = df.HelpfulnessNumerator.astype("int64")
    den = df.HelpfulnessDenominator.astype("int64")
    score = df.Score.astype("int64")
    ratio = num / den

    out.parent.mkdir(parents=True, exist_ok=True)
    pd.DataFrame({"ProductId": df.ProductId.astype(str), "HelpfulnessNumerator": num,
                  "HelpfulnessDenominator": den, "Score": score, "Ratio": ratio}
                 ).to_csv(out, index=False, float_format="%.9g")   # 9 sig. digits round-trips float32

    expected = out.with_suffix(".expected")
    expected.write_text(
        f"rows={len(df)}\nnum_sum={int(num.sum())}\nden_sum={int(den.sum())}\n"
        f"score_sum={int(score.sum())}\nratio_sum={float(ratio.sum())!r}\n"
        f"id_hash={id_hash(df.ProductId.astype(str))}\n")
    print(f"wrote {out} and {expected}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
