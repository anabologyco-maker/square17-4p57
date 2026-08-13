# Independent audit record — 2026-08-13

This document records an audit of the package performed after the v0.1.0 archive was
produced, before public release. **Transparency note:** the audits below were performed
by AI systems (Claude, Anthropic) — one primary auditor session plus three isolated agent
sessions with no shared code or state, each given a different adversarial brief. They are
*independent implementations and derivations*, but they are not human peer review, which
this package still requires.

## 1. Primary line-by-line audit

Scope: every file in the verification chain; the mathematical argument; the data.

- Re-ran the full pipeline from source on an independent machine: every stage passes;
  all counts (736,703 / 648,661 / 88,042 / 87,125 / 87,032 / 87,033), the per-chunk
  coverage statistics (118,392,770 runs), and both endpoint results reproduce exactly.
- Hand-derived the tan-half-angle algebra, the 40000·d² integer scaling of capture
  rectangles and feasible diamond, the four separating axes with projection radii, the
  ℤ[√2] endpoint frame, and the semantics of all three event families; confirmed each
  against the code by exact rational proportionality at six points (a degree-4 identity
  test). The completeness derivation is now written out in `EVENTS.md`.
- Recomputed the entire Bernstein prefilter in unbounded integers: max scaled numerator
  55,852,634,809,956 (165,137× inside int64), discard/keep split identical to the bundle.
  This closed the one real defect found: the released prefilter script computed in int64
  with an overflow check placed *after* the arithmetic (dead check). The script now
  computes in exact unbounded integers.
- Verified atom data directly: 408 atoms on the grid, all weights nonnegative, exact D4
  symmetry of the weighted set, exact mass 16,998,202,682,064/10¹².

## 2. Adversarial code audit (isolated agent)

Brief: break the verifiers — find any defect that could let a false certificate pass.
Method: differential fuzzing of every exact kernel against first-principles
reimplementations (240,783 segment-tree queries; 4,400 separating-axis cases including
tangencies; 5,711 ℤ(√2) endpoint cases; 3,000 exact-sign cases with near-ties to 10^200;
800 Sturm cases against sympy including planted endpoint factors and double roots — zero
mismatches anywhere), plus parser divergence probes, shell fault injection with fake
binaries, and end-to-end failure-path tests (planted root-at-sample, shared-gap roots,
all-zero rows: every scenario aborts nonzero).

Verdict: **sound**; no path to a false PASS. Five diagnostic-level findings, three of
which are fixed in this release: the chunk script now validates its parsed aggregate
fields, `verify_coverage_segment` rejects empty cell ranges instead of printing a
vacuous PASS when invoked standalone with out-of-range bounds, and the θ=0 endpoint
indexer gained the same missing-edge guard as the sweep verifier. The remaining two
(octal acceptance in `cpp_int` string parsing — moot for the regex-clean shipped data and
identical in both consumers; `std::terminate` on provably unreachable throws inside
OpenMP regions — fail-safe) were left as documented non-issues.

## 3. Independent mathematical review (isolated agent)

Brief: referee the mathematical chain adversarially; re-derive event completeness from
scratch. Verdict: **complete — no gaps**. The reviewer independently reproduced the
measure argument (identifying exactly where weight nonnegativity is load-bearing), the
D4 orientation reduction, the safety direction of the Bernstein superset interval, the
event-family derivation including the bounding-box tangency argument and the
identically-zero (persistent-incidence) cases, the left-monotone closure argument at
event roots, and the role of the gcd step in excluding two distinct roots per sample gap
(the exact failure mode that would leave an unaudited cell). Their probe of the audited
property at 40 adversarial orientations — including within 2·10⁻¹³ of event roots on
both sides and within 8·10⁻²⁰ of √2−1 — found it true everywhere. Their referee-style
requests (write out the completeness derivation; state both closure ingredients; note the
Sturm-bracket non-vanishing argument; explain incidences vs distinct roots; document
cross-file trust) are addressed in `EVENTS.md`, `PROOF.md`, and `AUDIT.md` in this
release.

## 4. From-scratch reimplementation (isolated agent)

Brief: re-verify a random sample of the package's claims with independently designed
exact code, sharing nothing with the package sources (data files only). Results, seed
457, zero discrepancies in 1,500 claims:

- **200 orientation cells** (endpoints of the index range, every 5000th, rest uniform):
  own rotated-frame derivation, integer edge grid at scale 800·d, exact prefix-sum
  coverage, own separating-axis feasibility test with sentinel padding — all 200 PASS,
  with independently computed minimum coverage exactly 1,000,400,000,008/10¹² at every
  tested orientation, matching the package's reported global minimum.
- **800 filtered event polynomials**: own Sturm/deflation machinery (validated against
  sympy) — 786 roots in (0, √2−1), every one strictly inside a sample gap, all in
  distinct gaps, none equal to a sample, none outside the first/last samples.
- **500 Bernstein-discarded polynomials**: proven root-free on the interval by direct
  exact root isolation (not Bernstein) — zero counterexamples. Also reconstructed the
  discard set: filtered rows occur exactly once in the raw file, complement 648,661.

## Remaining trust boundary after this audit

Sampling limits (the reimplementation covered 200/87,033 cells and subsets of the
polynomial checks; the primary pipeline covers everything but shares authorship),
compiler/hardware trust, and the fact that all auditors — however isolated — were
instances of the same AI system. Independent human review and, ideally, a fully
independent external implementation remain the bar for calling this a theorem. See
`LEAN_ROADMAP.md` for the formalization path that would retire most of what is left.
