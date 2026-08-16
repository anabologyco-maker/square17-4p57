import numpy as np, pandas as pd, math, json, sys
from scipy.optimize import linprog
from scipy.sparse import vstack, csr_matrix
S=4.57
support='/mnt/data/s17comp/compressed457_support52.csv'
base='/mnt/data/s17comp/compressed457_support52.npz'
auto='/mnt/data/s17comp/compressed457_automaton_min.csv'
out_support='/mnt/data/s17comp/compressed457_support52_iter.csv'
out_atoms='/mnt/data/s17comp/compressed457_atoms_rational_iter.csv'
D=pd.read_csv(support)
z=np.load(base,allow_pickle=True);A0=z['A'].item()[:,z['active']].astype(float);sizes=D.orbit_size.to_numpy(float)
a=pd.read_csv(auto)
# use first occurrence of each pattern
reps=a.sort_values('index').groupby('pattern_id',as_index=False).first().sort_values('pattern_id')

def orbit(x,y):
    pts={(x,y),(S-x,y),(x,S-y),(S-x,S-y),(y,x),(S-y,x),(y,S-x),(S-y,S-x)}
    return sorted(pts)
orbits=[orbit(float(r.rep_x),float(r.rep_y)) for _,r in D.iterrows()]

def row(q):
    x,y,t=q;c=math.cos(t);s=math.sin(t);out=[]
    for o in orbits:
        cnt=0
        for px,py in o:
            dx=px-x;dy=py-y;u=dx*c+dy*s;v=-dx*s+dy*c
            if abs(u)<.5-1e-8 and abs(v)<.5-1e-8:cnt+=1
        out.append(cnt)
    return np.array(out,float)
R=[]
for _,r in reps.iterrows():R.append(row((r.cx,r.cy,r.theta)))
R=np.vstack(R)
w0=D.weight.to_numpy(float)
print('pattern checks')
for i,r in reps.iterrows():
    val=R[int(r.pattern_id)]@w0
    print(int(r.pattern_id),val,r.min_weight_num/1e12,val-r.min_weight_num/1e12,R[int(r.pattern_id)].sum())
# dedup rows vs A0 and among R
existing={tuple(A0.getrow(i).toarray().ravel().astype(int)) for i in range(A0.shape[0])}
new=[]
for rr in R:
    k=tuple(rr.astype(int))
    if k not in existing:existing.add(k);new.append(rr)
A=vstack([A0,csr_matrix(np.vstack(new))],format='csr') if new else A0
res=linprog(sizes,A_ub=-A,b_ub=-np.ones(A.shape[0]),bounds=(0,None),method='highs',options={'primal_feasibility_tolerance':1e-10,'dual_feasibility_tolerance':1e-10})
print('solve',res.success,res.message,'mass',res.fun,'active',np.sum(res.x>1e-10),'rows',A.shape,'new',len(new),'min',float((A@res.x).min()))
D2=D.copy();D2['weight']=res.x;D2.to_csv(out_support,index=False)
# expand exact integer-grid D4 atoms, ceil weights to 1e12
K=1828;rows=[]
for oi,r in D2.iterrows():
    X=int(round(float(r.rep_x)*400));Y=int(round(float(r.rep_y)*400));pts={(X,Y),(K-X,Y),(X,K-Y),(K-X,K-Y),(Y,X),(K-Y,X),(Y,K-X),(K-Y,K-X)}
    num=int(math.ceil(float(r.weight)*1e12-1e-7))
    for x,y in sorted(pts):rows.append((x,y,int(r.orbit_index),num))
Aout=pd.DataFrame(rows,columns=['X','Y','orbit_index','weight_num']);Aout.to_csv(out_atoms,index=False)
print('rational mass',Aout.weight_num.sum()/1e12,'slack',17-Aout.weight_num.sum()/1e12,'atoms',len(Aout))
np.savez('/mnt/data/s17comp/compressed457_iter_state.npz',A=A,weights=res.x,sizes=sizes,new_rows=np.vstack(new) if new else np.empty((0,len(D))))
