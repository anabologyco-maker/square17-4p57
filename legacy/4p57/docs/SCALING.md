# Compression and scaling

The prior uncompressed 4.57 candidate used 76 support orbits, 600 atoms, and 1,534,120 raw event polynomials. The compressed machine uses 52 orbits and 408 atoms.

| stage | uncompressed candidate | compressed verified candidate |
|---|---:|---:|
| support orbits | 76 | 52 |
| expanded atoms | 600 | 408 |
| raw event polynomials | 1,534,120 | 736,703 |
| events after exact prefilter | 167,304 | 88,042 |
| distinct orientation roots | exploratory ~165,000 | exact 87,032 |
| orientation cells | exploratory ~165,001 | exact 87,033 |
| minimum-state automaton states | not compiled | 1 after feedback |

The combined support reduction and exact root prefilter reduce the root-isolation input by a factor of 17.42 relative to naively isolating every event of the 600-atom support.

Measured core-stage times in this environment:

- event regeneration and comparison: 12.4 s
- exact Bernstein prefilter audit: 2.5 s (unbounded-integer arithmetic)
- exact Sturm root partition: 8.3 s
- endpoint audit: below 0.01 s
- exact open-cell segment audits: 75.0 s summed across the successful 18-range run
- numerical minimum-state automaton: 26.8 s

Thus the exact core pipeline, excluding compilation and duplicated cross-checks, is roughly 97 seconds of recorded stage time despite auditing 87,033 orientation cells and 118,392,770 subthreshold runs.
