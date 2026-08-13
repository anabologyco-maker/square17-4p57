#include <omp.h>
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {
constexpr double SIDE = 4.57;
constexpr double INV_GRID = 1.0 / 400.0;
constexpr int WORDS = 7; // 7*64 >= 408
struct Atom { double x,y; std::int64_t w; };
struct Pt { double x,y; };
struct Result {
  std::int64_t minw = std::numeric_limits<std::int64_t>::max();
  std::array<std::uint64_t,WORDS> bits{};
  double t=0, theta=0, cx=0, cy=0;
  int slabs=0;
};

std::vector<std::string> split(const std::string& s) {
  std::vector<std::string> f; std::string c;
  for(char x:s){ if(x==','){f.push_back(c);c.clear();}else c.push_back(x);} f.push_back(c);return f;
}
std::vector<Atom> load_atoms(const std::string& p){
  std::ifstream in(p); if(!in) throw std::runtime_error("cannot open atoms");
  std::string line; std::getline(in,line); std::vector<Atom>a;
  while(std::getline(in,line)){if(line.empty())continue;auto f=split(line); if(f.size()<4)throw std::runtime_error("bad atom row");
    a.push_back({std::stod(f[0])*INV_GRID,std::stod(f[1])*INV_GRID,std::stoll(f[3])});}
  return a;
}
std::vector<double> load_samples(const std::string& p){std::ifstream in(p);if(!in)throw std::runtime_error("cannot open samples");std::vector<double>s;double x;while(in>>x)s.push_back(x);return s;}

struct SegTree {
  int n; std::vector<std::int64_t> mn,lazy; std::vector<int> arg;
  explicit SegTree(int n_):n(n_),mn(4*n_,0),lazy(4*n_,0),arg(4*n_,0){build(1,0,n);}
  void build(int p,int l,int r){if(r-l==1){arg[p]=l;return;}int m=(l+r)/2;build(2*p,l,m);build(2*p+1,m,r);pull(p);}
  void apply(int p,std::int64_t v){mn[p]+=v;lazy[p]+=v;}
  void push(int p){if(lazy[p]){apply(2*p,lazy[p]);apply(2*p+1,lazy[p]);lazy[p]=0;}}
  void pull(int p){if(mn[2*p]<=mn[2*p+1]){mn[p]=mn[2*p];arg[p]=arg[2*p];}else{mn[p]=mn[2*p+1];arg[p]=arg[2*p+1];}}
  void add(int ql,int qr,std::int64_t v){if(ql<qr)add(1,0,n,ql,qr,v);}
  void add(int p,int l,int r,int ql,int qr,std::int64_t v){if(qr<=l||r<=ql)return;if(ql<=l&&r<=qr){apply(p,v);return;}push(p);int m=(l+r)/2;add(2*p,l,m,ql,qr,v);add(2*p+1,m,r,ql,qr,v);pull(p);}
  std::pair<std::int64_t,int> query(int ql,int qr){return query(1,0,n,ql,qr);}
  std::pair<std::int64_t,int> query(int p,int l,int r,int ql,int qr){if(qr<=l||r<=ql)return {std::numeric_limits<std::int64_t>::max()/4,-1};if(ql<=l&&r<=qr)return {mn[p],arg[p]};push(p);int m=(l+r)/2;auto a=query(2*p,l,m,ql,qr);auto b=query(2*p+1,m,r,ql,qr);return a.first<=b.first?a:b;}
};

std::vector<Pt> clip_axis(const std::vector<Pt>& poly,int axis,double bound,bool ge){
  std::vector<Pt> out;if(poly.empty())return out;
  auto coord=[&](const Pt&p){return axis==0?p.x:p.y;};
  auto inside=[&](const Pt&p){return ge?coord(p)>=bound-1e-13:coord(p)<=bound+1e-13;};
  for(size_t i=0;i<poly.size();++i){Pt A=poly[i],B=poly[(i+1)%poly.size()];bool ia=inside(A),ib=inside(B);if(ia)out.push_back(A);if(ia!=ib){double da=coord(A),db=coord(B);double q=(bound-da)/(db-da);out.push_back({A.x+q*(B.x-A.x),A.y+q*(B.y-A.y)});}}
  return out;
}
std::vector<Pt> clip_box(std::vector<Pt> p,double xl,double xr,double yl,double yr){p=clip_axis(p,0,xl,true);p=clip_axis(p,0,xr,false);p=clip_axis(p,1,yl,true);p=clip_axis(p,1,yr,false);return p;}
std::pair<double,double> v_projection_strip(const std::vector<Pt>& square,double xl,double xr){
  auto p=clip_axis(square,0,xl,true);p=clip_axis(p,0,xr,false);if(p.empty())return {1,0};double lo=1e100,hi=-1e100;for(auto z:p){lo=std::min(lo,z.y);hi=std::max(hi,z.y);}return {lo,hi};
}
double polygon_area(const std::vector<Pt>& p){if(p.size()<3)return 0;double a=0;for(size_t i=0;i<p.size();++i){auto A=p[i],B=p[(i+1)%p.size()];a+=A.x*B.y-A.y*B.x;}return std::abs(a)*.5;}
Pt polygon_centroid(const std::vector<Pt>& p){double x=0,y=0;for(auto z:p){x+=z.x;y+=z.y;}return {x/p.size(),y/p.size()};}

void unique_sort(std::vector<double>& v){std::sort(v.begin(),v.end());std::vector<double> o;o.reserve(v.size());for(double x:v){if(o.empty()||std::abs(x-o.back())>1e-11)o.push_back(x);else o.back()=(o.back()+x)/2;}v.swap(o);}
int edge_index(const std::vector<double>& e,double x){auto it=std::lower_bound(e.begin(),e.end(),x-1e-10);int k=int(it-e.begin());if(k<(int)e.size()&&std::abs(e[k]-x)<2e-9)return k;if(k>0&&std::abs(e[k-1]-x)<2e-9)return k-1;throw std::runtime_error("edge index failure");}

Result audit_one(double t,const std::vector<Atom>& atoms){
  Result R;R.t=t;double den=1+t*t,c=(1-t*t)/den,s=2*t/den;R.theta=std::atan2(s,c);
  double rad=.5*(c+s),lo=rad,hi=SIDE-rad;
  // Feasible square corners transformed to u,v.
  std::vector<Pt> dom;dom.reserve(4);
  for(auto [x,y]:std::array<std::pair<double,double>,4>{{{lo,lo},{hi,lo},{hi,hi},{lo,hi}}})dom.push_back({x*c+y*s,-x*s+y*c});
  std::vector<double> xb,yb;xb.reserve(2*atoms.size()+4);yb.reserve(2*atoms.size()+4);
  struct Rect{double l,r,b,t;std::int64_t w;int yi0,yi1;};std::vector<Rect> rect;rect.reserve(atoms.size());
  for(auto a:atoms){double u=a.x*c+a.y*s,v=-a.x*s+a.y*c;rect.push_back({u-.5,u+.5,v-.5,v+.5,a.w,0,0});xb.push_back(u-.5);xb.push_back(u+.5);yb.push_back(v-.5);yb.push_back(v+.5);}
  for(auto p:dom){xb.push_back(p.x);yb.push_back(p.y);}unique_sort(xb);unique_sort(yb);
  if(xb.size()<2||yb.size()<2)throw std::runtime_error("bad edges");
  const int nx=xb.size()-1,ny=yb.size()-1;
  struct Ev{int l,r;std::int64_t d;};std::vector<std::vector<Ev>> ev(xb.size());
  for(auto &q:rect){q.yi0=edge_index(yb,q.b);q.yi1=edge_index(yb,q.t);int il=edge_index(xb,q.l),ir=edge_index(xb,q.r);ev[il].push_back({q.yi0,q.yi1,q.w});ev[ir].push_back({q.yi0,q.yi1,-q.w});}
  SegTree st(ny);
  for(int i=0;i<nx;++i){for(auto e:ev[i])st.add(e.l,e.r,e.d);double xl=xb[i],xr=xb[i+1];if(xr-xl<1e-14)continue;auto pr=v_projection_strip(dom,xl,xr);if(pr.first>pr.second)continue;++R.slabs;
    // y cells with nonempty open intersection with projected feasible interval.
    int ql=int(std::upper_bound(yb.begin(),yb.end(),pr.first+1e-12)-yb.begin())-1;ql=std::max(0,std::min(ny-1,ql));
    int qr=int(std::lower_bound(yb.begin(),yb.end(),pr.second-1e-12)-yb.begin());qr=std::max(ql+1,std::min(ny,qr));
    constexpr std::int64_t BIG = 4'000'000'000'000'000LL;
    std::vector<int> skipped;
    for(int attempt=0;attempt<8;++attempt){
      auto z=st.query(ql,qr);if(z.second<0||z.first>=BIG/2)break;int j=z.second;auto inter=clip_box(dom,xl,xr,yb[j],yb[j+1]);
      if(polygon_area(inter)<=1e-17){st.add(j,j+1,BIG);skipped.push_back(j);continue;}
      if(z.first<R.minw){Pt uv=polygon_centroid(inter);double px=uv.x*c-uv.y*s,py=uv.x*s+uv.y*c;
        std::array<std::uint64_t,WORDS> bits{};std::int64_t sum=0;for(int k=0;k<(int)atoms.size();++k){double dx=atoms[k].x-px,dy=atoms[k].y-py;double U=dx*c+dy*s,V=-dx*s+dy*c;if(std::abs(U)<.5-2e-10&&std::abs(V)<.5-2e-10){bits[k/64]|=std::uint64_t(1)<<(k%64);sum+=atoms[k].w;}}
        if(sum!=z.first) throw std::runtime_error("interior pattern/segment coverage mismatch");
        R.minw=z.first;R.bits=bits;R.cx=px;R.cy=py;
      }
      break;
    }
    for(int j:skipped)st.add(j,j+1,-BIG);
  }
  return R;
}

struct KeyHash{size_t operator()(const std::array<std::uint64_t,WORDS>&a)const noexcept{size_t h=1469598103934665603ULL;for(auto x:a){h^=x;h*=1099511628211ULL;}return h;}};
int hamming(const std::array<std::uint64_t,WORDS>&a,const std::array<std::uint64_t,WORDS>&b){int z=0;for(int i=0;i<WORDS;++i)z+=std::popcount(a[i]^b[i]);return z;}
}

int main(int argc,char**argv){try{
  std::string ap=argc>1?argv[1]:"compressed457_atoms_rational.csv";std::string sp=argc>2?argv[2]:"compressed457_samples_double.txt";int threads=argc>3?std::stoi(argv[3]):8;omp_set_num_threads(threads);
  auto atoms=load_atoms(ap);auto samples=load_samples(sp);std::cerr<<"atoms="<<atoms.size()<<" samples="<<samples.size()<<" threads="<<threads<<"\n";
  std::vector<Result> res(samples.size());double t0=omp_get_wtime();
#pragma omp parallel for schedule(dynamic,20)
  for(int i=0;i<(int)samples.size();++i){res[i]=audit_one(samples[i],atoms);if(i%10000==0){
#pragma omp critical
    std::cerr<<"done "<<i<<" sec="<<(omp_get_wtime()-t0)<<"\n";}}
  double sec=omp_get_wtime()-t0;
  std::unordered_map<std::array<std::uint64_t,WORDS>,int,KeyHash> ids;std::vector<int> seq;seq.reserve(res.size());std::int64_t global=std::numeric_limits<std::int64_t>::max();int gi=-1;long long slabs=0;
  for(int i=0;i<(int)res.size();++i){auto it=ids.find(res[i].bits);int id;if(it==ids.end()){id=ids.size();ids.emplace(res[i].bits,id);}else id=it->second;seq.push_back(id);if(res[i].minw<global){global=res[i].minw;gi=i;}slabs+=res[i].slabs;}
  int runs=res.empty()?0:1;std::map<int,long long> hd;for(int i=1;i<(int)res.size();++i){if(seq[i]!=seq[i-1])++runs;hd[hamming(res[i-1].bits,res[i].bits)]++;}
  std::ofstream out("/mnt/data/s17comp/compressed457_automaton_min.csv");out<<"index,t,theta,min_weight_num,pattern_id,cx,cy,slabs\n";out<<std::setprecision(17);for(int i=0;i<(int)res.size();++i)out<<i<<','<<res[i].t<<','<<res[i].theta<<','<<res[i].minw<<','<<seq[i]<<','<<res[i].cx<<','<<res[i].cy<<','<<res[i].slabs<<'\n';
  std::ofstream js("/mnt/data/s17comp/compressed457_automaton_summary.json");js<<"{\n  \"atoms\": "<<atoms.size()<<",\n  \"samples\": "<<samples.size()<<",\n  \"unique_min_patterns\": "<<ids.size()<<",\n  \"pattern_runs\": "<<runs<<",\n  \"global_min_weight_num\": "<<global<<",\n  \"global_min_sample\": "<<gi<<",\n  \"global_min_t\": "<<std::setprecision(17)<<res[gi].t<<",\n  \"global_min_theta\": "<<res[gi].theta<<",\n  \"global_min_center\": ["<<res[gi].cx<<", "<<res[gi].cy<<"],\n  \"total_slabs\": "<<slabs<<",\n  \"seconds\": "<<sec<<",\n  \"hamming_histogram\": {";bool first=true;for(auto [k,v]:hd){if(!first)js<<',';first=false;js<<"\n    \""<<k<<"\": "<<v;}js<<"\n  }\n}\n";
  std::cout<<"samples="<<samples.size()<<" unique_patterns="<<ids.size()<<" runs="<<runs<<" global_min="<<global<<" at="<<gi<<" total_slabs="<<slabs<<" sec="<<sec<<"\n";for(auto[k,v]:hd)std::cout<<"hamming "<<k<<" "<<v<<"\n";
  return 0;
}catch(const std::exception&e){std::cerr<<"error: "<<e.what()<<"\n";return 2;}}
