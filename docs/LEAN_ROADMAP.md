# Toward a Lean certificate

This note scopes what it would take to turn the 4.57 certificate into a Lean 4 / mathlib
formalization, and which route we would actually recommend. Nothing here is formalized yet.

## The proof splits into three layers

**Layer 1 — the measure argument.** From "every closed unit square contained in
`C=[0,457/100]^2` captures mass `>= 1`" and "total mass `< 17`" to "no 17 interior-disjoint
unit squares fit in a square of side `< 457/100`". This is textbook mathlib material:
finite atomic measures, a generic-translation lemma (the bad translation set is a finite
union of segments, hence null), and pigeonhole. Low risk, small effort, and worth doing
first regardless of everything else — it pins down the exact theorem statement.

**Layer 2 — orientation reduction.** The claim that the finite audit (87,033 sampled
orientations + two endpoints + closure at 87,032 event roots) implies the coverage
property at *every* orientation. Mathematically this is the event-completeness theorem:
on any open `t`-interval avoiding all roots of the three generated polynomial families,
the truth of the audited property is constant. This is the hardest thing to formalize
directly: it is a statement about combinatorial stability of an arrangement of 408 moving
squares plus a moving diamond, and none of the supporting theory (arrangements, Sturm
chains) is in mathlib today.

**Layer 3 — the finite computation.** Exact rational/integer checks: certificate
arithmetic, Bernstein sign conditions, Sturm root partition, per-sample coverage sweeps.
All decidable propositions over `ℚ`. In Lean these can be discharged by `native_decide`
(compiled evaluation; adds the compiler to the trust base) or by a verified checker
replaying certificate files through the kernel (slower, cleaner). The coverage stage is
~10^8 elementary rational comparisons — heavy for the kernel, plausible for
`native_decide`, and reducible further with per-cell hint certificates (e.g. recording
which separating axis discharges each low run, so the checker verifies rather than
searches).

## Recommended route: MVP first, then decide

**MVP ("Lean modulo one lemma").** Formalize Layers 1 and 3, and state Layer 2 as a
single explicitly-named geometric lemma (`event_completeness`), assumed. This already
replaces the entire C++/Python trust base with kernel-checked computation and leaves
exactly one crisply stated mathematical assumption for a human referee — a large,
honest improvement over "trust 1,100 lines of code". Estimated effort for a fluent
formalizer: person-weeks for Layer 1, person-months for Layer 3 (Sturm's theorem must be
proved from scratch; the coverage replay needs an efficient verified checker).

**Full formalization options for Layer 2:**

- *Direct route.* Formalize the completeness argument as reviewed here (edge-order
  changes = family A; diamond-corner/rectangle-edge incidences = family B; diamond-edge
  through arrangement vertices = family C; box-clipping transitions reduce to family B
  because the sweep box is tangent to the diamond at its corners). Bounded but genuinely
  hard; research-level formalization.

- *Restructured route: interval certificates.* Avoid events entirely: cover `t`-space by
  closed rational intervals; on each interval verify coverage with capture squares shrunk
  to their intersection over the interval and the feasible diamond enlarged to its union
  (all monotone, rational-endpoint arithmetic — trivially formalizable). Caveat discovered
  during this audit: near event roots where a subthreshold face approaches tangency with
  the diamond, separation margins go to zero, so a pure inflate/deflate certificate needs
  unboundedly fine intervals there. It can only work as a hybrid (interval certificates
  away from roots + a formalized local argument at/near roots), and the interval density
  needed should be measured empirically *before* committing to this route.

## Honest bottom line

A full Lean certificate is a serious but tractable project — think one strong formalizer
for a semester, most of it on Layers 2–3 — and the MVP is worth doing on its own. If the
result is to be publicized as machine-checked, the MVP wording must be precise about what
is and is not assumed.
