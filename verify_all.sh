#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"; cd "$ROOT"
mkdir -p bin verification-logs
CXXFLAGS='-O3 -std=c++17 -Wall -Wextra -Wpedantic'
g++ $CXXFLAGS -fopenmp -Isrc src/verify_event_partition.cpp -o bin/verify_event_partition
g++ $CXXFLAGS src/verify_endpoints.cpp -o bin/verify_endpoints
g++ $CXXFLAGS -fopenmp src/verify_coverage_segment.cpp -o bin/verify_coverage_segment
python scripts/check_certificate.py | tee verification-logs/certificate.log
python scripts/regenerate_events.py data/atoms.csv data/event_polys.npy | tee verification-logs/event_regeneration.log
python scripts/verify_bernstein_prefilter.py data/event_polys.npy data/event_polys_filtered.tsv | tee verification-logs/bernstein.log
bin/verify_endpoints data/atoms.csv | tee verification-logs/endpoints.log
bin/verify_event_partition 8 data/event_polys_filtered.tsv data/orientation_samples.tsv | tee verification-logs/event_partition.log
scripts/run_coverage_chunks.sh | tee verification-logs/coverage_master.log
echo 'ALL COMPRESSED 4.57 VERIFICATION STAGES PASSED'
