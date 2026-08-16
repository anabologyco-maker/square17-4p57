# Audit results

## Certificate

- orbit representatives: 52
- expanded atoms: 408
- coordinate grid: 1/400
- weight denominator: 10^12
- exact mass: 16.998202682064
- exact slack below 17: 0.001797317936

## Event geometry

- independently regenerated raw events: 736,703
- exact Bernstein discards: 648,661
- events sent to Sturm verification: 88,042
- polynomial root incidences: 87,125
- distinct interior roots: 87,032
- open orientation cells: 87,033

Incidences exceed distinct roots because several event polynomials can share one root;
the verifier proves all roots landing in the same sample gap are equal via exact
polynomial gcds, which is how 87,125 incidences collapse to exactly 87,032 distinct
roots. The Sturm counts use the rational upper bracket `4142135623730950489/10^19`,
proven above `sqrt(2)-1` by exact sign; no reduced event polynomial can vanish there,
since a primitive integer polynomial with the rational root `p/10^19`, `gcd(p,10^19)=1`,
needs its leading coefficient divisible by `10^19`, far above the coefficient heights in
this family.

Exact event verifier output:

```text
located_incidences=87125
root_gaps=87032 gcd_checks=87125 common=1
EXACT EVENT PARTITION PASS
```

## Coverage

- exact open cells audited: 87,033
- maximal subthreshold arrangement runs checked: 118,392,770
- failures: 0
- endpoint theta=0 minimum open-cell coverage: 1.000400000008
- endpoint theta=pi/4 subthreshold runs: 592, all outside the feasible diamond

The minimum arrangement coverage is zero because the audit includes arrangement regions outside the feasible-center domain. Every subthreshold region that can geometrically meet the feasible-center domain would cause the verifier to fail.

## Automaton feedback

The original 52-orbit compression had mass 16.989270288008 but the first full automaton sweep found a missing capture state of coverage 0.983600177056. Adding eight missing states raised the mass to 16.990740227532. A second sweep found one state at 0.996728583330. Adding it raised the mass to 16.991406119576. After multiplying all weights by 1.0004 and rounding upward, the exact certificate mass became 16.998202682064 and all 87,033 sampled cells shared one minimum-capture state with coverage 1.000400000008.

## Trust boundary

The finite arithmetic has passed the bundled exact verifiers. The remaining trust
boundary is source-code correctness and the mathematical derivation of the three event
families; the latter is now written out in `EVENTS.md`. Two conventions matter for
standalone use: the coverage and endpoint binaries deliberately do not re-validate weight
nonnegativity or the orbit structure of `atoms.csv` — that is `check_certificate.py`'s
job, and the pipeline is sound as a whole because every stage reads the same hash-pinned
files (`SHA256SUMS`). Running an individual binary against a different atoms file proves
nothing by itself. Independent external review is still required before treating this as
an established published theorem.
