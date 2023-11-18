#include<bits/stdc++.h>
#include <bitset>
#include <cassert>
#include <chrono>
#include <climits>
#include <queue>
using namespace std;
const int maxn = 10000;
const int maxm = 200000;
const int maxq = 100000;
const int maxp = 500;
const int inf = 0x3f3f3f3f;
int seed=11;//11(59356466) 1(59959677)
/*
存输入相关的数据结构
*/
int N,M,T,P,D;//输入的参数，节点数量N、连边数量M、业务数量T、单边通道数量 P、最大衰减距离D
int tmp_p;
int kk;
int k_path_num;
//输入边的结构体
struct edge_input{
    int u,v;//边
    int w;//另外一个可变变量
    edge_input(){}
    edge_input(int u,int v,int w):u(u),v(v),w(w){}
};
//对查询排序的一个结构体
struct query_input{
    int u,v,id;
    int num;//需要波长长度
    //cnt1表示业务重复数，cnt2表示最短路经过的波长跳数
    int cnt1,cnt2,cnt3;
    //按cnt2从大到小，然后cnt1从小到大
    bool f_lianxu;//该业务是否需要连续波长,true连续；false 不连续
    query_input(){}
    query_input(int u,int v,int id,int num,bool f_lianxu):u(u),v(v),id(id),num(num),f_lianxu(f_lianxu){}
    //按cnt2从大到小，然后cnt1从小到大:5347
    //按cnt2从大到小，然后cnt1从大到小:5313
    //按cnt4从大到小，按cnt2从大到小，然后cnt1从大到小:5319
    //按cnt3从大到小，按cnt2从大到小，然后cnt1从大到小:5347
    //波长递增：5278
    //加边，无限制版本按照广度优先求解。4892
    
};



inline bool cmd_query_cnt1(const query_input& a,const query_input& b){
    return a.cnt1 > b.cnt1;
}

inline bool cmd_query_cnt2(const query_input& a,const query_input& b){
    return a.cnt2 > b.cnt2;
}
inline bool cmd_query_cnt2_cnt1(const query_input& a,const query_input& b){
    if(a.cnt2==b.cnt2)return a.cnt1 > b.cnt1;
    return a.cnt2 > b.cnt2;
}

inline bool cmd_query_num(const query_input& a,const query_input& b){
    return a.num > b.num;
}
inline bool cmd_query_cnt2_num_cnt1(const query_input& a,const query_input& b){
    if(a.cnt2==b.cnt2){
        if(a.num==b.num)return a.cnt1 > b.cnt1;
        return a.num > b.num;
    }
    else{
        return a.cnt2>b.cnt2;
    }

}
inline bool cmd_query_num_cnt2_cnt1(const query_input& a,const query_input& b){
    if(a.num==b.num){
        if(a.cnt2==b.cnt2)return a.cnt1 > b.cnt1;
        return a.cnt2 > b.cnt2;
    }
    else{
        return a.num>b.num;
    }

}


struct work_answer{
    int m_cnt,n_cnt;
    vector<int> p_id;
    vector<int>ans_edge_id;
    vector<int>ans_router;
};

struct addedge{//记录加边的两个端点
    int u,v;
    addedge(){}
    addedge(int u,int v):u(u),v(v){}
};

map< pair<int, int>, int>mp;//记录边u-v的最小权值
vector<bitset<maxn> >conflict;//计算每条业务经过边的冲突度

/*
存图相关的数据结构
*/
struct edge{
    int u,v,w,next,curw;
    int id;//id表示当前边在输入中的顺序
    int pos;//pos表示当前边在链式前向星中的下标
    int curp[maxp];//记录当前边波长的占用情况
    int limitp;//当前的最高波长
    bitset<maxp> pset;//当前边波长占用情况，求集合
    int loadnum;//记录当前边所经过的业务数
};
int head[maxn],headcopy[maxn];
edge e[maxm<<1],ecopy[maxm<<1];//记录图中边的集合
int cnt;//记录边的个数，边是成对出现0和1、2和3
int maxedgew;//存储图中最大的边权值


/*
最短路部分的数据结构
*/
//用于记录求最短路过程中的前驱节点
struct node{
    int u,id;
};
int vis[maxn];//用于最短路过程中标记每个点
int dis[maxn];//带波长限制的最短路dis
int dis1[maxn];//不带限制的最短路dis
//记录最短路中的路径
node pre[maxn];


/*
功能：初始化边节点，清空所有边。
*/
inline void init(){
	memset(head,-1,sizeof(head));
	cnt = 0;
}

/*
功能：把图复制一份，head->headcopy，用于最终求路由放置。
*/
inline void updateedge(int n,int cnt){
    for(int i = 0;i < n;i++){
        headcopy[i] = head[i];
    }
    for(int i = 0;i < cnt;i++){
        ecopy[i] = e[i];
    }
}
int k_short_path(int s,int t,int k,vector<vector<int>>& ans_edge);

/*
功能：新增边，一条u->v的有向边
参数：u、v表示这条边的两个端点，w代表这条边的权值，id对应这条边在输入中的顺序
*/
inline void add(int u,int v,int w,int id){
	e[cnt].next = head[u];
    e[cnt].u = u;e[cnt].v = v;
	e[cnt].w = w;e[cnt].id = id;
    for(int i=0;i<P;i++){
        e[cnt].pset[i]=1;
        e[cnt].curp[i]=-1;
    }
    e[cnt].pos = cnt;head[u] = cnt++; 
}



/*
波长分配：为合理的路径分配波长
*/
inline int select_max(bitset<maxp> t_pset,int num){
    int count=0;
    int index=0;
    int count_max=0;
    for(int i=0;i<P;i++){
        int tpi = i;
        if(t_pset[i]==1){
            count = 0;
            while(t_pset[i]==1){
                i++;
                count++;
            }
            if(count>=num&&count>count_max){
                count_max = count;
                index = tpi;
            }
        }
    }
    return index;
}


/*
求解满足要求且最短的一个
*/
inline int select_min(bitset<maxp> t_pset,int num){
    int count=0;
    int index=0;
    int count_min=INT_MAX;
    for(int i=0;i<P;i++){
        int tpi = i;
        if(t_pset[i]==1){
            count = 0;
            while(t_pset[i]==1){
                i++;
                count++;
            }
            if(count>=num&&count<count_min){
                count_min = count;
                index = tpi;
            }
        }
    }
    return index;
}

/*
求解第一个满足的
*/
inline int select_fisrt(bitset<maxp> t_pset,int num){
    int count=0;
    int index=0;
    for(int i=0;i<P;i++){
        int tpi = i;
        if(t_pset[i]==1){
            count = 0;
            while(t_pset[i]==1){
                i++;
                count++;
            }
            if(count>=num){
                return tpi;
            }
        }
    }
    cout<<"errer"<<endl;
    return -1;
}

/*
求解满足中的随机一个
*/
inline int select_r(bitset<maxp> t_pset,int num){
    int count=0;
    int index=-1;
    for(int i=0;i<P;i++){
        int tpi = i;
        if(t_pset[i]==1){
            count = 0;
            while(t_pset[i]==1){
                i++;
                count++;
            }
            if(count>=num){
                if(index==-1)index = tpi;
                srand(seed++);
                if(rand()%2)index = tpi;
            }
        }
    }
    return index;
}


//连续版本
inline void allocation_sp_lianxu(bitset<maxp> t_pset,vector<int>& pset,int num){
    int index=select_max(t_pset,num);
    //int index=select_first(bitset<maxp> t_pset,num);
    //int index=select_min(bitset<maxp> t_pset,num);
    //int index=select_r(bitset<maxp> t_pset,num);
    pset.clear();
    for(int i = index;i < (num+index);i++){
        pset.push_back(i);
    }
    
}


bool swap_1(vector<int>& a,vector<int>& b,bool f){
//a业务和b业务是否可以撤回
    unordered_set<int> m;
    unordered_set<int> m1;
    for(auto& i:a){
        m.insert(i);
    }
    for(auto& i:b){
        if(m.count(i)){
            m1.insert(i);
        }
    }
    int n=ceil(min(a.size(),b.size())/2.0);
    if(f){
        if(m1.size()>n){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        if(m1.size()<n){
            return true;
        }
        else{
            return false;
        }
    }
    
}
//并集
bool swap_2(vector<int>& a,vector<int>& b,bool f){
//a业务和b业务是否可以撤回
    unordered_set<int> m;
    for(auto& i:a){
        m.insert(i);
    }
    for(auto& i:b){
        m.insert(i);
    }
    int n=max(a.size(),b.size())*1.5;
    if(f){
        if(m.size()>n){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        if(m.size()<n){
            return true;
        }
        else{
            return false;
        }
    }
    
}

//重复边数量
bool swap_3(vector<int>& a,vector<int>& b,bool f){
    unordered_set<int> m;
    unordered_set<int> m1;
    for(auto& i:a){
        m.insert(i);
    }
    for(auto& i:b){
        if(m.count(i)){
            m1.insert(i);
        }
    }
    int n=10;
    if(f){
        if(m1.size()>n){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        if(m1.size()<n){
            return true;
        }
        else{
            return false;
        }
    }
}

struct spfa_muti_node{
    int u,dis;
    bitset<maxp> pset;
    vector<int>path;
    bool operator < (const spfa_muti_node &A)const{
        return dis > A.dis;
    }
    spfa_muti_node() {}
    spfa_muti_node(int _u,int _dis,bitset<maxp>_pset):u(_u),dis(_dis),pset(_pset){}
    spfa_muti_node(int _u,int _dis,bitset<maxp>_pset,vector<int>_path):u(_u),dis(_dis),pset(_pset),path(_path){}
}d[maxn],d_tmp[maxn];
/*
功能：给业务s-t找到最短路，要求最短路径上的公共波长集合大小大于k，并把路径经过的边放到ans_edge里,pset表示可用波长编号,f_lianxu表示该业务是否需要连续波长，true 连续，fasle不连续
参数：s-t表示业务的起点-终点，ans_edge表示当前求的最短路，
返回值：1表示成功找到一条路径，0表示未找到一条路径，连通图应该必然存在一条路径
*/
int spfa_muti(int s,int t,vector<int>&ans_edge,vector<int>&pset,int k,bool f_lianxu){
    ans_edge.clear();
    memset(vis,0,sizeof(int)*N);
    for(int i = 0;i < N;i++){
        d[i].dis = inf;
    }
    // queue<int>q;
    // q.push(s);
    priority_queue<spfa_muti_node>q;
    d[s].dis = 0;
    for(int i = 0;i < P;i++)d[s].pset[i] = 1;
    q.push(spfa_muti_node(s,d[s].dis,d[s].pset));
    pre[s].u = -1;
    while(q.size()>0){
        spfa_muti_node now = q.top();
        int u = now.u;
        q.pop();
        if(vis[now.u])continue;
        vis[u] = 1;
        for(int i = head[u];i != -1;i = e[i].next){
            int v = e[i].v;
            int w = e[i].w;
            w = w + 10*e[i].loadnum;
            bitset<maxp>v_pset = (now.pset&e[i].pset);
            
            if(f_lianxu){
                int count=0;
                for(int i=0;i<P;i++){
                    if(v_pset[i]==1){
                        count++;
                    }
                    else{
                        if(count>=k){
                            break;
                        }
                        count=0;
                    }
                }
                if(count<k){
                    continue;
                }
            }
            else{
                if(v_pset.count()<k)continue;
            }
            if(d[v].dis > d[u].dis + w){
                //cout << v_pset << endl;
                d[v].dis = d[u].dis + w;
                d[v].pset = v_pset;
                pre[v].u = u;
                pre[v].id = e[i].pos;
                q.push(spfa_muti_node(v,d[v].dis,d[v].pset));
            }
        }
    }
    if(d[t].dis==inf)return 0;

    //得到最短路径
    int curv = t;
    while(pre[curv].u!=-1){
        ans_edge.push_back(pre[curv].id);
        curv = pre[curv].u;
    }
    reverse(ans_edge.begin(),ans_edge.end());

    //求最短路径上公共波长
    if(f_lianxu){
        allocation_sp_lianxu(d[t].pset,pset,k);
    }
    else{
       // allocation_sp_nolianxu(d[t].pset,pset,k);
    }
    // bitset<maxp>t_pset = d[t].pset;
    // for(int i = 0;i < maxp;i++){
    //     if(t_pset[i]==1){
    //         pset.push_back(i);
    //     }
    // }
    return 1;
}

/*
功能：给业务s-t找到最短路，要求最短路径上的公共波长集合大小大于k，并把路径经过的边放到ans_edge里,pset表示可用波长编号,f_lianxu表示该业务是否需要连续波长，true
连续，fasle不连续 参数：s-t表示业务的起点-终点，ans_edge表示当前求的最短路，
返回值：1表示成功找到一条路径，0表示未找到一条路径，连通图应该必然存在一条路径
*/
int spfa_muti1(int s,int t,vector<int>&ans_edge,vector<int>&pset,int k,bool f_lianxu){
    ans_edge.clear();
    memset(vis,0,sizeof(int)*N);
    for(int i = 0;i < N;i++){
        d[i].dis = inf;
    }
    // queue<int>q;
    // q.push(s);
    priority_queue<spfa_muti_node> q;
    
    vector<int> path;
    bitset<maxp> finalpset;
    d[s].dis = 0;
    for(int i = 0;i < P;i++)d[s].pset[i] = 1;
    q.push(spfa_muti_node(s,d[s].dis,d[s].pset,path));
    while(q.size()>0){
        spfa_muti_node now = q.top();
        vector<int>nowpath = now.path;
        int u = now.u;
        q.pop();
        if (u == t) {
            ans_edge = nowpath;
            finalpset = now.pset;
            break;
        }
        if(vis[now.u]>kk)continue;
        vis[u]++;
        for(int i = head[u];i != -1;i = e[i].next){
            int v = e[i].v;
            int w = e[i].w;
            w = w + 10*e[i].loadnum;
            bitset<maxp>v_pset = (now.pset&e[i].pset);
            if(f_lianxu){
                int count=0;
                for(int i=0;i<P;i++){
                    if(v_pset[i]==1){
                        count++;
                    }
                    else{
                        if(count>=k){
                            break;
                        }
                        count=0;
                    }
                }
                if(count<k){
                    continue;
                }
            }
            else{
                if(v_pset.count()<k)continue;
            }
            bool flag = 1;
            for (int j = 0; j < nowpath.size(); j++) {
                if(e[nowpath[j]].u == v){
                    flag = 0;
                    break;
                }
            }
            if(!flag)continue;
            vector<int> newpath = nowpath;
            newpath.push_back(i);
            d[v].dis = d[u].dis + w;
            d[v].pset = v_pset;
            pre[v].u = u;
            pre[v].id = e[i].pos;
            q.push(spfa_muti_node(v, d[v].dis, d[v].pset, newpath));
            if(u==t){
                ans_edge = newpath;
                finalpset = v_pset;
                break;
            }
        }
    }
    if(d[t].dis==inf)return 0;
    // for (int i = 0; i < ans_edge.size(); i++) {
    //     cout << e[ans_edge[i]].u << " " << e[ans_edge[i]].v << endl;
    // }
    // cout << finalpset << endl;

    //求最短路径上公共波长
    if(f_lianxu){
        allocation_sp_lianxu(finalpset,pset,k);
    }
    else{
       // allocation_sp_nolianxu(finalpset,pset,k);
    }
    // bitset<maxp>t_pset = d[t].pset;
    // for(int i = 0;i < maxp;i++){
    //     if(t_pset[i]==1){
    //         pset.push_back(i);
    //     }
    // }
    return 1;
}

/*
功能：给业务s-t找到最短路，没有波长的限制，并把路径经过的边放到ans_edge里
参数：s-t表示业务的起点-终点，ans_edge表示当前求的最短路，
返回值：1表示成功找到一条路径，0表示未找到一条路径，连通图应该必然存在一条路径
*/
int spfa1_normal(int s,int t,vector<int>&ans_edge){
    ans_edge.clear();
    memset(dis1,0x3f,sizeof(int)*N);
    memset(vis,0,sizeof(int)*N);
    dis1[s] = 0;
    vis[s] = 1;
    pre[s].u = -1;
    queue<int>q;
    q.push(s);
    while(q.size()>0){
        int u = q.front();
        q.pop();
        vis[u] = 0;
        for(int i = head[u];i != -1;i = e[i].next){
            int w=1;
            if(dis1[e[i].v]>dis1[e[i].u]+w){
                dis1[e[i].v] = dis1[e[i].u] + w;
                pre[e[i].v].id = e[i].pos;
                pre[e[i].v].u = e[i].u;
                if(vis[e[i].v]==0){
                    q.push(e[i].v);
                    vis[e[i].v] = 1;
                }
            }
        }
    }

    if(dis1[t]==inf)return 0;
    int curv = t;
    //printf("pre:%d\n",curv);
    while(pre[curv].u!=-1){
       // assert(pre[curv].id>=0&&pre[curv].id<cnt);
        ans_edge.push_back(pre[curv].id);
        //printf("edge:%d\n",pre[curv].id);
        curv = pre[curv].u;
       // printf("pre:%d\n",curv);
    }
    reverse(ans_edge.begin(),ans_edge.end());
    return 1;
}


/*
功能：为ans_edge里的边求一个路由放置，从前往后或者从后往前贪心的选即可。
参数：ans_edge为要分配路由的路径，bignum为分配的路径节点
*/
void getbig(const vector<int>&ans_edge,vector<int> &bignum){
    int sum = 0;
    vector<int>ans[3];
    for(int i = 0;i < ans_edge.size();i++){
        sum += e[ans_edge[i]].w;
        if(sum>D){
            sum = e[ans_edge[i]].w;
            ans[0].push_back(e[ans_edge[i]].u);
        }
    }
    sum = 0;
    for(int i = ans_edge.size() - 1;i >= 0;i--){
        sum += e[ans_edge[i]].w;
        if(sum>D){
            sum = e[ans_edge[i]].w;
            ans[1].push_back(e[ans_edge[i]].v);
        }
    }
    if(ans[0].size()<ans[1].size())bignum = ans[0];
    else bignum = ans[1];
}


/*
功能：与getbig类似，不过这里是求最终的路径的路由放置。
参数：ans_edge为要分配路由的路径，bignum为分配的路径节点
*/
void getbigcopy(const vector<int>&ans_edge,vector<int> &bignum){
    bignum.clear();
    int sum = 0;
    vector<int>ans[3];
    for(int i = 0;i < ans_edge.size();i++){
        sum += ecopy[ans_edge[i]].w;
        if(sum>D){
            sum = ecopy[ans_edge[i]].w;
            ans[0].push_back(ecopy[ans_edge[i]].u);
        }
    }
    sum = 0;
    for(int i = ans_edge.size() - 1;i >= 0;i--){
        sum += ecopy[ans_edge[i]].w;
        if(sum>D){
            sum = ecopy[ans_edge[i]].w;
            ans[1].push_back(ecopy[ans_edge[i]].v);
        }
    }
    if(ans[0].size()<ans[1].size())bignum = ans[0];
    else bignum = ans[1];
}


bool checkfile(const char *filename){
    FILE *fp=fopen(filename,"r");
    if(fp!=NULL){
        fclose(fp);
        return 1;
    }
    return 0;
}

/*
功能：处理所有的输入，并计算最大的边权值和记录每条边的最小权值
参数：edge 表示输入的边集合，query表示输入的业务集合
*/
void readfile(const char* pathin, vector<edge_input> &edge,vector<query_input>&query){
    freopen(pathin,"r",stdin);
    cin>>N>>M>>T>>P;
    for(int i = 0;i < M;++i){
        int u,v,w;
        cin>>u>>v;
        w = 1;
        edge.push_back(edge_input(u,v,w));
        maxedgew = max(maxedgew,w);
        //记录边u-v的最小权值
        pair<int, int>tp = make_pair(min(u,v), max(v,u));
        if(mp.count(tp)){
            mp[tp] = min(mp[tp],w);
        }else mp[tp] = w;
    }
    for(int i = 0;i < T;++i){
        int u,v,num;
        cin>>u>>v>>num;
        query.push_back(query_input(u,v,i,num,true));
    }
}


/*
功能：把输入的边构建成一个图
参数：要构建图的边集合
*/
void constructgraph(const vector<edge_input> &edge){
    //初始化图的数据结构
    memset(e,0,sizeof(e));
    init();
    assert(edge.size()==M);
    //开始构建图
    for(size_t i = 0;i < edge.size();++i){
        int u,v,w;
        u = edge[i].u;
        v = edge[i].v;
        w = edge[i].w;
        add(u,v,w,i);
        
        //这里一开始为0貌似会更好

        //e[cnt-1].curw = maxedgew - e[cnt-1].w;
        add(v,u,w,i);
        //e[cnt-1].curw = maxedgew - e[cnt-1].w;
    }

}

/*
功能：统计某一条业务下经过的路径情况
参数：
*/
void cal_conflict(const vector<int> &ans_edge){
    bitset<maxn>edge_conflict;
    for(size_t i = 0;i < ans_edge.size();i++){
        edge_conflict[e[ans_edge[i]].id] = 1;
    }
    conflict.push_back(edge_conflict);
}

/*
使用前提：图必须要建好，需要求路径数
功能：计算业务的排序指标 cnt1、cnt2、cnt3
参数：
*/
struct NL{
    vector<int> Nolimit_path;
    int dis1[maxn];
};
map<pair<int,int>,NL> Nolimit;
void calworkorders(vector<query_input>& query){
    map<pair<int, int> ,int> work;//业务重复次数
    assert(query.size()==T);
    for(size_t i = 0;i < query.size();++i){
        int u,v;
        u = query[i].u;
        v = query[i].v;
        work[make_pair(min(u,v), max(u,v))]++;
        
    }
    conflict.clear();
    for(size_t i = 0;i < query.size();++i){
        int u,v;
        u = query[i].u;
        v = query[i].v;
        auto tmp=make_pair(u,v);
        auto tmp1=make_pair(v,u);
        query[i].cnt1 = work[make_pair(min(u,v), max(u,v))];
        if(Nolimit.count(tmp)==0){
            vector<int>ans_edge;
            spfa1_normal(u,v,ans_edge);//不带限制的最短路权重为1.
            NL tmp2;
            tmp2.Nolimit_path=ans_edge;
            std::copy(dis1, dis1 + maxn, tmp2.dis1); // 将 arr1 的元素复制到 arr2 中
            Nolimit.insert(make_pair(tmp,tmp2));
            query[i].cnt2 = ans_edge.size() + 1;
        }
        else{
            query[i].cnt2 = Nolimit[tmp].Nolimit_path.size() + 1;
        }
        //cal_conflict(ans_edge);
        
    }

    //计算每个业务与其他业务在没有波长限制的条件下的冲突度
    // for(size_t i = 0;i < conflict.size();++i){
    //     int conflict_cnt = 0;
    //     for(size_t j = 0;j < conflict.size();++j){
    //         if(i==j)continue;
    //         if((conflict[i]&conflict[j])!=0)conflict_cnt++;
    //     }
    //     query[i].cnt3 = conflict_cnt;
    // }

    sort(query.begin(),query.end(),cmd_query_num_cnt2_cnt1);
}

/*
功能：撤销path里所有边波长为aimp的负载。
参数：path为需要撤回的边集合，aimp表示波长。
*/
void un_do(const vector<int>& path,vector<int>& aimp){
    //撤销边的负载和波长
    for(auto& i:path){
        for(auto& j:aimp){
            e[i].curp[j]=-1;//未使用
            e[i].loadnum--;
            e[i].pset.set(j,1);
            if(i&1){
                e[i-1].curp[j] = -1;
                e[i-1].loadnum--;
                e[i-1].pset.set(j,1);
            }else {
                e[i+1].curp[j] = -1;
                e[i+1].loadnum--;
                e[i+1].pset.set(j,1);
            }
        }
        
    }
}

/*
功能：重新分配path里所有边波长为aimp的负载，分配给q_业务。
参数：path为需要撤回的边集合，aimp表示波长，q_此次分配的业务。
*/
void _do(const vector<int>& path,vector<int>& aimp,int q_){
    //分配边的负载和波长
    for(auto& i:path){
        for(auto& j:aimp){
            e[i].curp[j]=q_;//使用
            e[i].loadnum++;
            assert(e[i].pset[j]==1);
            e[i].pset[j] = 0;
            if(i&1){
                e[i-1].curp[j] = q_;
                e[i-1].loadnum++;
                assert(e[i-1].pset[j]==1);
                e[i-1].pset[j] = 0;
            }else {
                e[i+1].curp[j] = q_;
                e[i+1].loadnum++;
                assert(e[i+1].pset[j]==1);
                e[i+1].pset[j] = 0;
            }
        }  
    }
}



/*交换业务*/
bool rollback(const vector<query_input> &query,int k,vector<int>& yewu,vector<vector<int>>& work_path,vector<vector<int>>& RFL){
    //将撤回业务，先全部撤回；再将新业务路由（路由顺序）；
    bool f1=false;
    int u=query[k].u;
    int v=query[k].v;
    //撤销
    for(auto& qq:yewu){
        auto tmp=work_path[query[qq].id];
        auto rfl_p=RFL[query[qq].id];
        un_do(tmp,rfl_p);
    }
    //先排当前业务，用撤回的波长
    vector<int> ans_edgetmp;
    vector<int> rfl_tmp;
    //求当前业务的路由，带限制版本;返回路径和波长
    int f1_ = spfa_muti1(u,v,ans_edgetmp,rfl_tmp,query[k].num,query[k].f_lianxu);
    if(f1_){
        //当前波长可以有新的路径，you试着将其分配，并将撤回的业务重新分配。
        _do(ans_edgetmp,rfl_tmp,k);
        bool f__=false;
        vector<vector<int> > work_path_tmp;//记录新的路径
        vector<vector<int> > RFL_tmp;
        for(auto& qq_tmp:yewu){
            //重新分配
            vector<int> ans_edgetmp1;
            vector<int> rfl_tmp1;
            int uu=query[qq_tmp].u;
            int vv=query[qq_tmp].v;
            int f_=spfa_muti1(uu,vv,ans_edgetmp1,rfl_tmp1,query[qq_tmp].num,query[qq_tmp].f_lianxu);
            if(f_==0){
                //该业务重新分配失败
                f__=true;
                break;
            }
            else{
                //当前业务成功，还要继续分配
                work_path_tmp.emplace_back(ans_edgetmp1);
                RFL_tmp.emplace_back(rfl_tmp1);
                _do(ans_edgetmp1,rfl_tmp1,qq_tmp);
            }
        }
        if(f__){
            //相同波长业务分配失败，需要回滚到最初状态
            un_do(ans_edgetmp,rfl_tmp);
            for(int i=0;i<work_path_tmp.size();i++){
                //对相关波长业务，进行回滚
                un_do(work_path_tmp[i],RFL_tmp[i]);
            }
            for(auto& qq_tmp:yewu){
                _do(work_path[query[qq_tmp].id],RFL[query[qq_tmp].id],qq_tmp);
            }
        }
        else{
            //重新分配成功
            f1=true;
            for(int i=0;i<work_path_tmp.size();i++){
                RFL[query[yewu[i]].id]=RFL_tmp[i];
                work_path[query[yewu[i]].id]=work_path_tmp[i];
            }
            RFL[query[k].id]=rfl_tmp;
            work_path[query[k].id]=ans_edgetmp;
            return f1;
        }
    }
    else{
        //还原之前的撤销
        for(auto& qq:yewu){
            auto tmp=work_path[query[qq].id];
            auto rfl_p=RFL[query[qq].id];
            _do(tmp,rfl_p,qq);
        }
    }

    return f1;
}

/*交换*/
//query:业务
//k:当前业务
//work_path:路由
//RFL：波长集合
//flag:最短or最长路
//成功返回true
//不成功返回false
bool demand_swap(const vector<query_input> &query,int k,vector<vector<int> >& work_path,vector<vector<int> >& RFL){
    bool f1=false;
    vector<int> qq_tmp(k+1);
    for(int i=0;i<=k;i++){
        qq_tmp[i]=i;
    }
    srand(seed++);
    random_shuffle(qq_tmp.begin(), qq_tmp.end());
    //业务占用波长从大到小：
    int len_tmp=qq_tmp.size();
    for(int qqq=0;qqq<len_tmp;qqq++){
        vector<int> yewu_tmp{qq_tmp[qqq]};
        f1=rollback(query,k,yewu_tmp,work_path,RFL);
        if(f1){
            return f1;
        }
    }
    for(int qqq=0;qqq<len_tmp/10;qqq++){
        for(int qqqq=qqq+1;qqqq<len_tmp/10;qqqq++){
            // if(!swap_1(RFL[query[qq_tmp[qqq]].id],RFL[query[qq_tmp[qqqq]].id],true)){
            //     continue;
            // }
            vector<int> yewu_tmp{qq_tmp[qqq],qq_tmp[qqqq]};
            f1=rollback(query,k,yewu_tmp,work_path,RFL);
            if(f1){
                break;
            }
        }
        if(f1){
            break;
        }
    }  
    return f1;
}



struct astarnode{
    int u;
    int f,g;//f表示实际估算，g表示已经走过的距离
    vector<int>path;
    astarnode(){}
    astarnode(int u,int f,int g,const vector<int>&path):u(u),f(f),g(g),path(path){}
    bool operator < (const astarnode &A)const{
        return f > A.f;
    }
};
int h[maxn];
int k_short_path(int s,int t,int k,vector<vector<int> >&ans_edge){
    auto tmp=make_pair(t,s);
    if(Nolimit.count(tmp)==1){
        for(int i = 0;i < N;i++){
            h[i] = Nolimit[tmp].dis1[i];
        }
    }
    else{
        vector<int>res_dege;
        int sta = spfa1_normal(t,s,res_dege);
        if(sta==0)return 0;
        NL tmp1;
        tmp1.Nolimit_path=res_dege;
        std::copy(dis1, dis1 + maxn, tmp1.dis1); // 将 arr1 的元素复制到 arr2 中
        Nolimit.insert(make_pair(tmp,tmp1));
        for(int i = 0;i < N;i++){
            h[i] = dis1[i];
        }
    }
    //到这一步表示s到t有路径可达，不管有没有限制
    priority_queue<astarnode>q;
    vector<int>path;
    q.push(astarnode(s,h[s],0,path));
    int cnt = 0;
    int cursize = 0;
    while(q.size()>0&&cnt<k){
        astarnode now = q.top();
        q.pop();
        if(now.u==t){ 
            set<int>sp;// 去重，判断每条边只出现一次
            for(size_t i = 0;i < now.path.size();i++){
                sp.insert(e[now.path[i]].id);
            }
            int pre = ans_edge.size();
            assert(sp.size()==now.path.size());
            ++cnt;
            ans_edge.push_back(now.path);
            //cout <<"边大小：" <<ans_edge.size() << endl;
        }
        //cout << "u:" << now.u << endl;
        for(int i = head[now.u];i != -1;i = e[i].next){
            // if(limits!=-1){
            //     if(e[i].curp[limits])continue;
            // }
            int v = e[i].v;
            int g = now.g + e[i].w;
            int f = g + h[v];
            path = now.path;
            bool flag = 1;
            for (int j = 0; j < now.path.size(); j++) {
                if(e[now.path[j]].u == v)flag = 0;
            }
            if (flag) {
                path.push_back(e[i].pos);
                q.push(astarnode(v,f,g,path));
            }
        }
    }
    
    return 1;
}
/*交换*/
//query:业务
//k:当前业务
//work_path:路由
//RFL：波长集合
//flag:最短or最长路
//成功返回true
//不成功返回false
bool demand_swap_1(const vector<query_input> &query,int k,vector<vector<int> >& work_path,vector<vector<int> >& RFL){
    bool f1=false;
    vector<vector<int>> nolimitans_edge;
    vector<int> limitans_edge;
    vector<int> limitans_rfl;
    k_short_path(query[k].u,query[k].v,k_path_num,nolimitans_edge);
    //auto f=spfa_muti1(query[k].u,query[k].v,limitans_edge,limitans_rfl,query[k].num,query[k].f_lianxu);
    vector<int> cut_edge;
    for(int i=0;i<nolimitans_edge.size();i++){
        for(int ie=0;ie<nolimitans_edge[i].size();ie++){
            int uu=e[nolimitans_edge[i][ie]].u;
            int vv=e[nolimitans_edge[i][ie]].v;
            if(d[uu].dis!=inf&&dis1[vv]!=inf&&d[vv].dis==inf){
                cut_edge.push_back(nolimitans_edge[i][ie]);
            }
        }
    }
    set<int> qq_tmp_1;
    vector<int> qq_tmp;
    for(auto& i:cut_edge){
        for(int j=0;j<P;j++){
            if(e[i].curp[j]!=-1){
                qq_tmp_1.insert(e[i].curp[j]);
            }
        }
    }
    for(auto& i:qq_tmp_1){
        qq_tmp.push_back(i);
    }
    
    srand(seed++);
    random_shuffle(qq_tmp.begin(), qq_tmp.end());
    //业务占用波长从大到小：
    int len_tmp=qq_tmp.size();
    for(int qqq=0;qqq<len_tmp;qqq++){
        vector<int> yewu_tmp{qq_tmp[qqq]};
        f1=rollback(query,k,yewu_tmp,work_path,RFL);
        if(f1){
            return f1;
        }
    }
    // for(int qqq=0;qqq<len_tmp/10;qqq++){
    //     for(int qqqq=qqq+1;qqqq<len_tmp/10;qqqq++){
    //         // if(!swap_1(RFL[query[qq_tmp[qqq]].id],RFL[query[qq_tmp[qqqq]].id],true)){
    //         //     continue;
    //         // }
    //         vector<int> yewu_tmp{qq_tmp[qqq],qq_tmp[qqqq]};
    //         f1=rollback(query,k,yewu_tmp,work_path,RFL);
    //         if(f1){
    //             break;
    //         }
    //     }
    //     if(f1){
    //         break;
    //     }
    // }  
    return f1;
}



void solution_normal(const vector<edge_input> &edge,const vector<query_input> &query,
    vector<addedge> &add_edge,vector<vector<int> > &work_path,vector<vector<int> > &RFL){
    work_path.clear();
    work_path.resize(T+1);
    RFL.clear();
    RFL.resize(T+1);
    //重新初始化图
    constructgraph(edge);
    int curm = M;
    int flag = 1;
    assert(query.size()==T);
    for(int k = 0;k < T;k++){
        int u = query[k].u;
        int v = query[k].v;
        vector<int>ans_edge;
        vector<int>ans_rfl;
        vector<vector<int> >ans_edges;
        vector<vector<int> >ans_rfls;
        bool multi_path=false;
        int f=spfa_muti1(u,v,ans_edge,ans_rfl,query[k].num,query[k].f_lianxu);
        std::copy(d, d + maxn, d_tmp); // 将 arr1 的元素复制到 arr2 中
        //交换
        if(f==0){
            bool f1=demand_swap_1(query,k,work_path,RFL);
            if(f1){
                continue;
            }
        }
        //cout << "第" << k << "条业务" << endl;
        if(f==1){
            //路由成功
            assert(ans_rfl.size()==query[k].num);
            RFL[query[k].id]=ans_rfl;
            work_path[query[k].id] = ans_edge;
            //cout << 1 << endl;
            _do(ans_edge, ans_rfl, k);
            //cout << 1 << endl;
        }else{
            //加边
            
            vector<int>limitans_edge;
            vector<int>limitans_rfl;
            //spfa1_normal(u, v, nolimitans_edge);
            auto uv=make_pair(u,v);
            assert(Nolimit.count(uv)==1);
            vector<int>nolimitans_edge = Nolimit[uv].Nolimit_path;
            //f = spfa_muti1(u,v,limitans_edge,limitans_rfl,query[k].num,query[k].f_lianxu);

            addedge curadd;
            int curmaxloadnum = -1;
            for(int ie = 0;ie < nolimitans_edge.size();ie++){
                int uu = e[nolimitans_edge[ie]].u;
                int vv = e[nolimitans_edge[ie]].v;
                if(d_tmp[uu].dis!=inf&&Nolimit[uv].dis1[vv]!=inf&&d_tmp[vv].dis==inf){
                    if(e[nolimitans_edge[ie]].loadnum>curmaxloadnum){
                        curadd=addedge(uu,vv);
                        curmaxloadnum = e[nolimitans_edge[ie]].loadnum;
                    }  
                }
            }
            
            int uu,vv;
            uu = curadd.u;
            vv = curadd.v;
            //cout<<uu<<" "<<vv<<endl;
            add(uu,vv,mp[make_pair(min(uu,vv), max(uu,vv))],curm);
                // curm++;
            add(vv,uu,mp[make_pair(min(uu,vv), max(uu,vv))],curm);
           // e[cnt-1].curw = maxedgew - e[cnt-1].w;
            //e[cnt-2].curw = maxedgew - e[cnt-2].w;
            curm++;
            add_edge.push_back(curadd);
            k--;
            //cout << 2 << endl;
        }
    }

}


/*
功能：输出最终的答案
参数：新增的边，最终每条业务的答案
*/
void printanswer(const char* pathout, const vector<addedge> & add_edge,const vector<work_answer>& ans,const vector<query_input>&query){
    //输出新增的边
    freopen(pathout, "w",stdout);
    cout<<(int)add_edge.size()<<endl;
    for(size_t i = 0;i < add_edge.size();++i){
        printf("%d %d\n",add_edge[i].u,add_edge[i].v);
    }
    //输出每条业务的答案
    for(size_t i = 0;i < ans.size();++i){
        //printf("第 %d 条业务的情况：\n",(int)i);
       // printf("经过的编号: ");
        for(size_t j = 0;j < ans[i].p_id.size();++j){
            printf("%d ",ans[i].p_id[j]);
        }
       // printf("\n");
        // printf("%d %d ",ans[i].m_cnt,ans[i].n_cnt);
        //printf("经过的边的编号：");
        printf("%d ",(int)ans[i].ans_edge_id.size());
        for(size_t j = 0;j < ans[i].ans_edge_id.size();++j){
            printf("%d ",ans[i].ans_edge_id[j]);
        }
        // printf("\n");
        

        // for(size_t j = 0;j < ans[i].ans_router.size();++j){
        //     printf("%d ",ans[i].ans_router[j]);
        // }
        printf("\n");
    }
    freopen("CON","w",stdout);
}

int check(const char* pathin,const char* pathout){
    freopen(pathout,"r",stdin);
    int addEdgeNum;
    scanf("%d",&addEdgeNum);

    freopen(pathin,"r",stdin);
    int N,M,T,P; 
    scanf("%d%d%d%d",&N,&M,&T,&P);
    const int max_edge=M+addEdgeNum;
    vector<vector<int>> edge(max_edge,vector<int>(2));
    map<pair<int, int>, int> minEdge; 
    for(int i = 0;i < M;i++){
        int w=1;
        scanf("%d%d",&edge[i][0],&edge[i][1]);
    }

    int task[20000][3]; //业务起点 终点 和需要波长数量
    for(int i = 0;i < T;i++){
        scanf("%d%d%d",&task[i][0],&task[i][1],&task[i][2]);
    }
    
    freopen(pathout,"r",stdin);
    int addNum;
    scanf("%d",&addNum);
    for(int i = M;i < M + addNum;i++){
        scanf("%d%d",&edge[i][0],&edge[i][1]);
        if(edge[i][0] >=N || edge[i][1] >= N) {
            printf("error7: Illegal addition of edges.\n");
            printf("error2: edge%d Illegal point number.\n",i);
            return 0;
        }
    }

    vector<int> result_bochang[20010];
    vector<int> result_luyou[20010];
    vector<vector<int>> channel(M+addEdgeNum,vector<int>(P,0));//0未使用，1使用

    for (int i = 0; i < T; i++){
        vector<int> temp(task[i][2]);
        int pre_j=-1;
        for(int j=0;j<temp.size();j++){
            scanf("%d",&temp[j]);
            if(!(temp[j] >= 0 && temp[j] < P)) {
                printf("error4: task%d Illegal channel number.\n", i);
                return 0;
            }
            if(pre_j==-1){
                pre_j=temp[j];
            }
            else{
                if(pre_j+1!=temp[j]){
                    printf("error4: task%d no lianxu channel pre_j %d temp[j] %d lenth %d.\n", i,pre_j,temp[j],task[i][2]);
                    return 0;
                }
                else{
                    pre_j++;
                }
            }

        }
        

        result_bochang[i]=temp;
        int size_lu=0;
        scanf("%d",&size_lu);
        if(size_lu != 0){
            if(size_lu < 0 || size_lu > 25000) {
                printf("error5: task%d Illegal number of path edges.\n", i);
                return 0;
            }
        }
        vector<int> temp_lu(size_lu);
        for(int j=0;j<temp_lu.size();j++){
            scanf("%d",&temp_lu[j]);
            if(temp_lu[j] >= M + addNum) {
                printf("error3: task%d edge%d Illegal edge number.\n", i, j);
                return 0;
            }
        }
        result_luyou[i]=temp_lu;
    }
    for(int i=0;i<T;i++){
        if(task[i][0]==task[i][1]){
            continue;
        }
        int preNode = task[i][0];
        for(int j=0;j<result_luyou[i].size();j++){
            if(edge[result_luyou[i][j]][0] == preNode){
                preNode = edge[result_luyou[i][j]][1];
            }else if(edge[result_luyou[i][j]][1] == preNode){
                preNode = edge[result_luyou[i][j]][0];
            }else{
                printf("error8: task%d edge%d Path discontinuity.\n", i,j);
                return 0;
            }
            if((j == (result_luyou[i].size()-1))&&(preNode != task[i][1])){
                printf("error8: task%d last edge Path discontinuity.\n", i);
                return 0;
            }
            for (int k = 0; k < result_bochang[i].size(); k++)
            {
               if(channel[result_luyou[i][j]][result_bochang[i][k]] == 1){
                    printf("error11: task%d edge%d Channel%d Conflict.\n", i,j,k);
                    return 0;
                }
                else{
                    channel[result_luyou[i][j]][result_bochang[i][k]]=1;
                }
            } 
        }
    }
    
    //printf("check end!\n");
    return 1;
}


int main(){
    vector<string> name{"enron-only.in","infect-dublin.in","crime-moreno.in","email-univ.in"};
    vector<double> ans_fe;
    vector<double> ans_time;
    for(int ii=6;ii<=6;ii++){
        seed=11;
        auto start = chrono::steady_clock::now();
        auto ed = start;
        string pathin="E:\\c++\\paper\\ConsoleApplication1\\email-univ"+to_string(ii)+".in";
        string pathout="E:\\c++\\paper\\ConsoleApplication1\\JRSAFE_email-univ"+to_string(ii)+".out";
        vector<edge_input>edge_input;//存储输入的边 u-v表示边、w表示边权
        vector<query_input>query_input;//存储输入的查询 u-v表示边、id表示输入的顺序   
        //读取输入
        
        readfile(pathin.c_str(), edge_input,query_input);
        conflict.clear();
        //构建图
        constructgraph(edge_input);
        //业务排序
        calworkorders(query_input);
        vector<addedge> max_add_edge;
        vector<vector<int> > work_path_max;//业务路径规划
        vector<vector<int> > RFL_max;//业务的波长分配
        int max_add=INT_MAX;
        // print(edge_input,query_input);
        vector<int> ans_edge_tmp;
        vector<int> ans_rfl_tmp;
        int count=1;
        kk = 1;
        k_path_num=1;
        /*small
        1,1,1
        1,1,1
        1,1,2
        2,3,7
        2,5,4
        2,1,1
        1,3,1
        2,4,10
        2,2,1
        4,12,2
        1,2,2
        2,2,2
        2,1,1
        1,1,1
        2,1,1
        2,6,10
        2,1,1
        4,2,1
        8,3,1
        10,3,3
        2,2,3
        2,1,1
        2,1,1
        large
        
        1,1,1
        1,1,1
        */
        while(count--){
            vector<addedge>add_edge;
            vector<vector<int> >  work_path;
            vector<vector<int> > RFL;
            solution_normal(edge_input, query_input,add_edge,work_path, RFL);
            if((int)add_edge.size()<max_add){
                max_add_edge.clear();
                max_add_edge=add_edge;
                work_path_max.clear();
                work_path_max=work_path;
                max_add=(int)add_edge.size();
                RFL_max.clear();
                RFL_max=RFL;
                updateedge(N,cnt);
            }   
        }
        ed = chrono::steady_clock::now();
        chrono::duration<double> dif = ed - start;
        //cout << double(ed-st)/CLOCKS_PER_SEC << endl;
        //处理结果
        vector<work_answer> ans(T);
        for(int i = 0;i < T;++i){
            work_answer work_ans;
            work_ans.p_id = RFL_max[query_input[i].id];
            work_ans.m_cnt = work_path_max[query_input[i].id].size();
            vector<int>big;
            getbigcopy(work_path_max[query_input[i].id],big);
            work_ans.n_cnt = big.size();
            for(size_t j = 0;j < work_path_max[query_input[i].id].size();++j){
                work_ans.ans_edge_id.push_back(e[work_path_max[query_input[i].id][j]].id);
            }
            for(size_t j = 0;j < big.size();++j){
                work_ans.ans_router.push_back(big[j]);
            }
            ans[query_input[i].id] = work_ans;
        }

        //输出答案
        printanswer(pathout.c_str(), max_add_edge, ans, query_input);
        ans_fe.push_back(max_add);
        
        ans_time.push_back(dif.count());
       //  cout<<dif.count()<<endl;
        auto l=check(pathin.c_str(),pathout.c_str());
        if(l==0){
            cout<<"fail"<<endl;
        }
    }
    for(auto& i:ans_fe){
        cout<<i<<endl;
    }
    for(auto& i:ans_time){
        cout<<i<<endl;
    }
    
    
    return 0;
}

