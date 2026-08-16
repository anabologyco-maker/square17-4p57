import numpy as np,pandas as pd,math,os
from scipy.optimize import linprog
from scipy.sparse import vstack,csr_matrix
S=4.57;D=pd.read_csv('/mnt/data/s17comp/compressed457_support52.csv');sizes=D.orbit_size.to_numpy(float)
state='/mnt/data/s17comp/compressed457_iter_state.npz'
if os.path.exists(state):
 z=np.load(state,allow_pickle=True);A=z['A'].item() if z['A'].shape==() else csr_matrix(z['A']);
else:
 z=np.load('/mnt/data/s17comp/compressed457_support52.npz',allow_pickle=True);A=z['A'].item()[:,z['active']].astype(float)
a=pd.read_csv('/mnt/data/s17comp/compressed457_automaton_min.csv')
reps=a.sort_values('index').groupby('pattern_id',as_index=False).first().sort_values('pattern_id')
def orbit(x,y):return sorted({(x,y),(S-x,y),(x,S-y),(S-x,S-y),(y,x),(S-y,x),(y,S-x),(S-y,S-x)})
orbits=[orbit(float(r.rep_x),float(r.rep_y)) for _,r in D.iterrows()]
def row(x,y,t):
 c=math.cos(t);s=math.sin(t);v=[]
 for o in orbits:
  n=0
  for px,py in o:
   dx=px-x;dy=py-y;u=dx*c+dy*s;w=-dx*s+dy*c
   if abs(u)<.5-1e-8 and abs(w)<.5-1e-8:n+=1
  v.append(n)
 return np.array(v,float)
R=np.vstack([row(r.cx,r.cy,r.theta) for _,r in reps.iterrows()])
keys={tuple(A.getrow(i).toarray().ravel().astype(int)) for i in range(A.shape[0])};new=[]
for rr in R:
 k=tuple(rr.astype(int))
 if k not in keys:keys.add(k);new.append(rr)
if new:A=vstack([A,csr_matrix(np.vstack(new))],format='csr')
r=linprog(sizes,A_ub=-A,b_ub=-np.ones(A.shape[0]),bounds=(0,None),method='highs',options={'primal_feasibility_tolerance':1e-10,'dual_feasibility_tolerance':1e-10})
print('feedback new',len(new),'patterns',len(R),'rows',A.shape[0],'mass',r.fun,'active',sum(r.x>1e-10),'min',float((A@r.x).min()))
D2=D.copy();D2['weight']=r.x;D2.to_csv('/mnt/data/s17comp/compressed457_support52_iter.csv',index=False)
K=1828;out=[]
for _,q in D2.iterrows():
 X=int(round(q.rep_x*400));Y=int(round(q.rep_y*400));pts={(X,Y),(K-X,Y),(X,K-Y),(K-X,K-Y),(Y,X),(K-Y,X),(Y,K-X),(K-Y,K-X)};num=int(math.ceil(q.weight*1e12-1e-7))
 for x,y in sorted(pts):out.append((x,y,int(q.orbit_index),num))
pd.DataFrame(out,columns=['X','Y','orbit_index','weight_num']).to_csv('/mnt/data/s17comp/compressed457_atoms_rational_iter.csv',index=False)
rmass=sum(z[3] for z in out)/1e12
print('rational mass',rmass,'slack',17-rmass)
np.savez(state,A=A,weights=r.x,sizes=sizes)
