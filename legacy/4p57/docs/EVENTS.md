# Event completeness: why one sample per cell suffices

This note writes out the derivation that PROOF.md compresses into two sentences: the
audited property is piecewise-constant in orientation between roots of the three
generated event families, so verifying it at one exact rational orientation per open cell
(plus the endpoints, plus a closure argument at the roots) verifies it for every
orientation. This is the mathematical heart of the finite reduction; nothing here is
computed, it justifies what the computation covers.

## Setup

Fix `S = 457/100` and `C = [0,S]^2`. For orientation `theta` write `R = R_theta` for the
rotation and parametrize `t = tan(theta/2) in (0, sqrt(2)-1)` for `theta in (0, pi/4)`;
at rational `t = p/q` all of `cos theta = (q^2-p^2)/d`, `sin theta = 2pq/d`, `d = p^2+q^2`
are rational. Endpoints `theta = 0, pi/4` are audited by dedicated exact programs and play
no role here.

Work in the rotated center frame `u = R_{-theta} c`. For an atom `a`, the set of centers
whose closed unit square `Q(c,theta)` contains `a` is the closed axis-aligned unit square
`R_a(t)` centered at `R_{-theta} a` ("capture square"). The set of feasible centers
(`Q(c,theta) ⊆ C`) is `F(theta) = [h, S-h]^2` with `h = (cos theta + sin theta)/2`; its
image `D(t) = R_{-theta} F(theta)` is a rotated square ("diamond") with nonempty interior
for every `t` (since `S > sqrt 2`), whose edges are never axis-parallel for
`t in (0, sqrt(2)-1)`.

Let `P(t)` be the statement: *every point of `D(t)` is covered by capture squares of
total weight at least 1.*

## A clipping-free characterization of P

Weights are nonnegative and capture squares are closed, so coverage
`cov(w) = sum of weights of squares containing w` is upper semicontinuous in `w`; the set
`{cov < 1}` is open. Consider the full-plane arrangement of the 816 vertical and 816
horizontal capture-edge lines. Coverage is constant on each open face, and for a point `w`
on the 1-skeleton, `cov(w) >= cov(f)` for every open face `f` with `w` in its closure.
Hence:

> `P(t)` fails **iff** some open face `f` with `cov(f) < 1` meets `int D(t)`.

This characterization involves no sweep bounding box and no run merging — those are
verifier implementation details. (That the verifier decides exactly this at a sample is a
separate, code-level fact: the sweep grid restricted to the bounding box of `D` refines
the arrangement there, clipped cell coverage equals pointwise coverage on open cells, and
the strict 4-axis separating-axis test decides intersection of each low run's closed hull
with the closed diamond exactly; conservative in the failing direction throughout.)

## What can change P

By the characterization, the truth of `P` on an open `t`-interval is determined by:

1. **The weak order of the u-edge values and of the v-edge values.** All capture squares
   are unit-size, so the 816 edge values per axis are `(rotated atom coordinate) ± 1/2`.
   The order type fixes the face lattice of the arrangement and the exact coverage of
   every face (a face is covered by a square iff the square's edges bracket it, a
   relation determined by order and coincidence of edge values). An order change at some
   `t*` forces two edge values to be equal at `t*`, i.e. a difference of rotated
   coordinates hits `{0, +1, -1}` on the same axis. These are exactly the roots of
   **family A** (`z + dx cos theta + dy sin theta` and `z + dy cos theta - dx sin theta`,
   `z in {-1, 0, 1}` in grid units, over all atom pairs). The only identically-zero
   family-A instance is the trivial `dx = dy = 0, z = 0` (same atom), so distinct atoms
   never have persistently tied edges that could hide an order change.

2. **The strict verdict "face meets int D" for each face.** A transition forces a
   tangency between the closed face rectangle and the closed diamond at `t*`. Since
   diamond edges are never axis-parallel on the open interval, first contact between an
   axis-aligned rectangle and the diamond happens with either
   - a **diamond vertex on a rectangle edge line** — roots of **family B**: the rotated
     coordinates of the four corners of `F(theta)` equal a capture-edge value; all four
     corners × both coordinates × both edge offsets are generated for every atom; or
   - a **rectangle corner on a diamond edge line** — corners of arrangement faces are
     intersections of a vertical edge line of some atom `p` and a horizontal edge line of
     some atom `q`; **family C** generates, for all ordered pairs `(p,q)`, both edge
     offsets on each, and all four diamond edge lines, the incidence polynomial.
   Vertex–vertex contacts are simultaneous roots of family B and change nothing further.
   No irreducible three-way coincidence family is needed: at any transition instant a
   compound degeneracy still contains one of the pairwise incidences above, which is a
   root of A, B, or C at that same instant.

3. **The sweep bounding box.** The verifier clips to the bounding box of `D(t)`, whose
   four sides pass through the four extreme vertices of `D(t)` (the box is tangent to the
   diamond there). A capture edge entering or leaving the box therefore crosses a diamond
   vertex coordinate — a **family B** root. Grid vertices involving a box edge can touch
   the diamond only at that tangent vertex, again family B. So clipping introduces no
   events outside the generated families. (By the clipping-free characterization above
   this is strictly a statement about the verifier's decision procedure, not about `P`.)

4. **Persistent incidences.** The generator drops identically-zero polynomials. Family B
   instances are never identically zero (the coefficient conditions are inconsistent).
   Family C instances are identically zero exactly when an atom lies on the container
   boundary, in which case one capture-square corner rides a diamond edge line for *all*
   `t`; a persistent incidence is not a transition, and the associated genuine transition
   (the corner sliding past the diamond vertex) is a family B root. This certificate has
   no boundary atoms, so the case is vacuous here anyway.

Consequently `P` is constant on every open `t`-interval containing no root of any
generated polynomial. The Bernstein prefilter discards only polynomials with no root in
`(0, 83/200) ⊇ (0, sqrt(2)-1)` (one-signed, not-all-zero degree-4 Bernstein coefficients
on the superset interval; such a polynomial can vanish on the closed physical interval
only at `t = 0`, which the dedicated `theta = 0` audit covers). The partition verifier
proves the surviving 88,042 polynomials have exactly 87,032 distinct roots in
`(0, sqrt(2)-1)`, strictly interleaved with the 87,033 audited samples. So every open
cell of the true event partition contains an audited sample, and `P` holds on all open
cells.

## Event orientations: the closure argument

Let `t*` be an event root and `c` a feasible center at `t*`. Two facts finish the proof:

- **Feasibility is left-monotone:** `h(theta) = (cos theta + sin theta)/2` is
  nondecreasing on `[0, pi/4]`, so `F(theta)` shrinks as `theta` grows; `c` is feasible
  for every `theta <= theta*`. In particular `c` is feasible throughout the open cell
  immediately to the left of `t*` (such a cell always exists: the partition proves the
  gap below the first sample is root-free, so the first root has the cell
  `(0, r_1) ∋ s_1` on its left).
- **Upper semicontinuity in orientation:** for `theta_n ↑ theta*`, any atom outside the
  closed square `Q(c, theta*)` has positive distance to it and is eventually outside
  `Q(c, theta_n)`; with nonnegative weights,
  `mu(Q(c, theta*)) >= limsup mu(Q(c, theta_n)) >= 1`.

The left-sided approach is mandatory — a center feasible at `t*` need not be feasible to
the right — and it is all the argument uses. Together with the two endpoint audits this
covers every `theta in [0, pi/4]`, and D4 invariance of the certificate extends this to
all orientations.
