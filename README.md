# Compressed exact certificate candidate for packing 17 unit squares: side at least 4.57

This package contains an internally exact, finite computer-assisted proof candidate for

$$\boxed{\,s(17)\ \ge\ 4.57 = \tfrac{457}{100}\,}$$

i.e. no 17 pairwise interior-disjoint unit squares fit in a square of side less than 4.57.
The best known packing of 17 unit squares (Bidwell) has side ≈ 4.6756; the trivial area
lower bound is $\sqrt{17}\approx 4.1231$.

The certificate is a nonnegative atomic measure with 52 dihedral support orbits,
408 expanded atoms, coordinates in `(1/400) Z^2`, weights in `(1/10^12) Z`, and exact mass
`16.998202682064 < 17`. The finite audit proves that every closed unit square contained in
`C=[0,4.57]^2`, at every orientation, captures mass at least 1; seventeen interior-disjoint
unit squares would therefore need total mass at least 17.

The verification pipeline independently regenerates 736,703 geometric event polynomials,
exactly discards 648,661 root-free events using integer Bernstein coefficients, verifies an
exact 87,032-root orientation partition, checks both endpoint orientations, and audits all
87,033 open orientation cells using arbitrary-precision integer geometry. Every proof
decision is an exact integer or rational comparison; floating point appears only in
timing/reporting and in the exploratory tooling that *found* the certificate (which is not
part of the verification chain).

## Verifying

Requirements: `g++` (C++17, OpenMP), Boost headers (`libboost-dev`), `python3` with `numpy`.

```bash
./verify_all.sh
```

Expected: every stage prints an explicit `... PASS` line and the script ends with
`ALL COMPRESSED 4.57 VERIFICATION STAGES PASSED`. Full pipeline time is a few minutes on a
modern multicore machine; see `docs/SCALING.md` for per-stage timings.

The coverage stage is intentionally split into 18 process-isolated ranges and uses one
OpenMP thread per range. This avoids a nondeterministic scheduling stall observed in the
monolithic multithreaded executable; it does not change the arithmetic or the audited cell
set.

## Layout

- `docs/PROOF.md` — the mathematical argument and finite reduction.
- `docs/AUDIT.md` — audited quantities and pipeline results.
- `docs/INDEPENDENT_AUDIT.md` — results of an independent line-by-line audit and
  cross-checks (2026-08-13).
- `docs/SCALING.md` — compression statistics and stage timings.
- `docs/LEAN_ROADMAP.md` — scoping notes for a future Lean formalization.
- `src/`, `scripts/`, `verify_all.sh` — the verification chain.
- `data/` — certificate, event polynomials, orientation samples, bundled results.
- `logs/` — reference logs from the release environment.
- `SHA256SUMS` — checksums of all data, docs, scripts, and sources.

`scripts/reopt_from_automaton.py`, `scripts/automaton_feedback.py`, and
`src/automaton_min_sweep.cpp` are the floating-point exploratory tools used to construct
and compress the certificate. They are **not** invoked by `verify_all.sh` and nothing in
the proof depends on them; they are included for provenance.

## Provenance

- **Proof and pipeline**: found and constructed by GPT 5.6 Sol Pro (OpenAI).
- **Independent audit**: Claude Fable 5 (Anthropic) — line-by-line review plus three
  isolated adversarial agent audits; see `docs/INDEPENDENT_AUDIT.md`.
- **Release compilation**: Claude Fable 5.
- **External human peer review**: pending.

## Review status

All reported proof decisions are exact integer/rational decisions, the bundled pipeline
passes end-to-end, and the package has been audited line-by-line together with independent
recomputations of the prefilter, event-family semantics, and sampled cells (see
`docs/INDEPENDENT_AUDIT.md`). It has not yet been peer reviewed or reproduced by a fully
independent external implementation. Treat the result as a complete proof candidate
pending that review.
