# Proof outline

Let `C=[0,457/100]^2`. The certificate is a nonnegative atomic measure `mu` on `C` with exact mass

$$\mu(C)=\frac{16{,}998{,}202{,}682{,}064}{10^{12}}=16.998202682064<17.$$

The finite audit proves that every closed unit square `Q` contained in `C`, at every orientation, satisfies `mu(Q)>=1`.

If seventeen pairwise interior-disjoint unit squares fit in any smaller container, embed that container strictly inside `C` and apply an arbitrarily small generic common translation so no certificate atom lies on a packed-square boundary. The seventeen squares then capture disjoint atom sets, giving

$$17\le\sum_{i=1}^{17}\mu(Q_i)\le\mu(C)<17,$$

a contradiction.

## Finite reduction

By dihedral symmetry, orientations reduce to `0<=theta<=pi/4`. Set `t=tan(theta/2)`. The combinatorial overlay of atom-capture rectangles in rotated center space and the feasible-center square changes only at roots of three explicitly generated families of integer polynomials of degree at most four.

The support generates 736,703 primitive event polynomials. Integer Bernstein coefficients on `[0,83/200]` prove that 648,661 have no root on the open physical interval `(0,sqrt(2)-1)` (a discarded polynomial may still vanish at `t=0`; that orientation has its own dedicated audit). Exact Sturm sequences applied to the remaining 88,042 polynomials prove there are exactly 87,032 distinct interior roots and therefore 87,033 open orientation cells. The derivation that these three families exhaust every orientation at which the audited combinatorial structure can change — so that the audited property is constant on each open cell — is written out in `EVENTS.md`.

At one exact rational `t` inside each cell, the segment-tree coverage verifier constructs all transformed atom rectangles with arbitrary-precision integers. It sweeps their x-boundaries, maintains exact y-coverage by range additions, enumerates every maximal run whose coverage is below one, and proves each such run is strictly separated from the feasible-center square by one of four separating axes. Both endpoint orientations are checked separately in exact arithmetic.

Event orientations follow by a closure argument with two load-bearing ingredients. First, the feasible-center square `[h,457/100-h]^2`, `h=(cos theta+sin theta)/2`, shrinks as `theta` grows on `[0,pi/4]`, so a center feasible at an event orientation is feasible throughout the open cell immediately to its left. Second, capture is upper semicontinuous for closed squares with nonnegative weights: an atom outside the closed square at the event orientation is eventually outside along any left approach, so `mu(Q) >= 1` on the left cell passes to the limit. The left-sided approach is mandatory — feasibility can be lost on the right — and it is all the argument uses. See `EVENTS.md` for the full statement.
