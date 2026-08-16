import numpy as np, hashlib, sys
from math import comb
raw_path=sys.argv[1] if len(sys.argv)>1 else '/mnt/data/s17comp/compressed457_event_polys.npy'
filtered_path=sys.argv[2] if len(sys.argv)>2 else '/mnt/data/s17comp/compressed457_events_filtered.tsv'
A=np.load(raw_path)
if A.shape!=(736703,5): raise SystemExit(f'unexpected raw shape {A.shape}')
if not np.issubdtype(A.dtype,np.integer): raise SystemExit('raw events are not integers')
# Degree-4 Bernstein numerators on [0,83/200], scaled by harmless positive factors.
# Computed in unbounded Python integers, so overflow is impossible by construction.
p,q,L=83,200,12
Aexact=A.astype(object)
B=np.zeros((len(A),5),dtype=object)
for i in range(5):
    for k in range(i+1):
        B[:,i]+=Aexact[:,k]*(p**k)*(q**(4-k))*comb(i,k)*(L//comb(4,k))
max_abs=max(abs(int(v)) for v in B.ravel())
nonneg=(B>=0).astype(bool).all(1)
nonpos=(B<=0).astype(bool).all(1)
allzero=(B==0).astype(bool).all(1)
constant_sign=(nonneg|nonpos)&~allzero
F=A[~constant_sign].astype(np.int64)
bundled=np.loadtxt(filtered_path,dtype=np.int64,delimiter='\t')
if bundled.ndim==1:bundled=bundled[None,:]
match=F.shape==bundled.shape and np.array_equal(F,bundled)
print(f'raw_polynomials={len(A)}')
print(f'exactly_discarded={int(constant_sign.sum())}')
print(f'filtered_polynomials={len(F)}')
print(f'max_abs_bernstein_numerator={max_abs}')
print('arithmetic=exact_unbounded_integers')
print(f'int64_headroom_factor={(2**63-1)/max_abs:.1f}')
print(f'filtered_file_match={int(match)}')
print('raw_sha256='+hashlib.sha256(open(raw_path,'rb').read()).hexdigest())
print('filtered_sha256='+hashlib.sha256(open(filtered_path,'rb').read()).hexdigest())
if not match:raise SystemExit(1)
print('EXACT BERNSTEIN PREFILTER PASS')
