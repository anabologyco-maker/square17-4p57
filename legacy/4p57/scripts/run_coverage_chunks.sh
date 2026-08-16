#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/bin/verify_coverage_segment"
ATOMS="$ROOT/data/atoms.csv"
SAMPLES="$ROOT/data/orientation_samples.tsv"
OUT="$ROOT/verification-logs/coverage"
mkdir -p "$OUT"; : > "$ROOT/verification-logs/coverage_chunks.tsv"
total_cells=0;total_runs=0;global_min=9223372036854775807
for begin in $(seq 0 5000 85000); do
 end=$((begin+5000)); ((end>87033)) && end=87033
 log="$OUT/${begin}_${end}.log"
 "$BIN" 1 "$ATOMS" "$SAMPLES" "$begin" "$end" > "$log" 2>&1
 grep -q '^EXACT SEGMENT COVERAGE PASS$' "$log"
 line="$(grep '^cells=' "$log" | tail -1)"
 cells="$(sed -n 's/.*cells=\([0-9]*\).*/\1/p' <<<"$line")"
 runs="$(sed -n 's/.*runs=\([0-9]*\).*/\1/p' <<<"$line")"
 mincov="$(sed -n 's/.*min_arrangement_coverage=\([0-9]*\).*/\1/p' <<<"$line")"
 ok="$(sed -n 's/.*ok=\([01]\).*/\1/p' <<<"$line")"
 [[ "$cells" =~ ^[0-9]+$ && "$runs" =~ ^[0-9]+$ && "$mincov" =~ ^[0-9]+$ ]]
 [[ "$ok" == 1 ]]
 total_cells=$((total_cells+cells));total_runs=$((total_runs+runs));((mincov<global_min))&&global_min=$mincov
 printf '%d\t%d\t%s\n' "$begin" "$end" "$line" | tee -a "$ROOT/verification-logs/coverage_chunks.tsv"
done
[[ "$total_cells" == 87033 ]]
echo "aggregate_cells=$total_cells aggregate_runs=$total_runs global_min=$global_min"
echo 'EXACT CHUNKED SEGMENT COVERAGE PASS'
