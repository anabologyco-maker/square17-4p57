import csv,hashlib,sys
from pathlib import Path
root=Path(__file__).resolve().parents[1]
K=1828;GRID=400;DEN=10**12;EXPECTED_MASS=16998202682064
orbit_path=root/'data/certificate_orbits.csv';atom_path=root/'data/atoms.csv'
orbits=list(csv.DictReader(open(orbit_path)))
atoms=list(csv.DictReader(open(atom_path)))
def orbit(X,Y):
 return sorted(set([(X,Y),(K-X,Y),(X,K-Y),(K-X,K-Y),(Y,X),(K-Y,X),(Y,K-X),(K-Y,K-X)]))
expanded={}
for r in orbits:
 oid=int(r['orbit_index']);X=int(r['X']);Y=int(r['Y']);size=int(r['orbit_size']);w=int(r['weight_numerator']);den=int(r['weight_denominator']);om=int(r['orbit_mass_numerator'])
 if den!=DEN or w<0 or size*w!=om:raise SystemExit('bad orbit arithmetic')
 if not (0<=X<=K and 0<=Y<=K):raise SystemExit('orbit representative outside container grid')
 o=orbit(X,Y)
 if len(o)!=size:raise SystemExit('bad orbit size')
 for p in o:
  if p in expanded:raise SystemExit('duplicate atom')
  expanded[p]=(oid,w)
raw={(int(r['X']),int(r['Y'])):(int(r['orbit_index']),int(r['weight_num'])) for r in atoms}
if expanded!=raw:raise SystemExit('expanded atom table mismatch')
if not all(0<=x<=K and 0<=y<=K for x,y in expanded):raise SystemExit('atom outside container grid')
mass=sum(w for oid,w in expanded.values())
if mass!=EXPECTED_MASS or mass>=17*DEN:raise SystemExit('bad total mass')
print(f'orbit_representatives={len(orbits)}')
print(f'expanded_atoms={len(atoms)}')
print(f'grid_denominator={GRID}')
print(f'weight_denominator={DEN}')
print(f'exact_total_mass_numerator={mass}')
print(f'exact_total_mass={mass/DEN:.12f}')
print(f'exact_mass_slack_numerator={17*DEN-mass}')
print(f'exact_mass_slack={(17*DEN-mass)/DEN:.12f}')
print('orbit_table_sha256='+hashlib.sha256(open(orbit_path,'rb').read()).hexdigest())
print('expanded_atoms_sha256='+hashlib.sha256(open(atom_path,'rb').read()).hexdigest())
print('EXACT CERTIFICATE ARITHMETIC PASS')
