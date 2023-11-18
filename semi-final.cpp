#include<bits/stdc++.h>
#include <cassert>
#include <climits>
#include <queue>
using namespace std;
const int maxn = 5000 + 10;
const int maxm = 2e5 + 10;
const int maxq = 10000 + 10;
const int maxp = 80 + 10;
const int inf = 0x3f3f3f3f;

/*
存输入相关的数据结构
*/
int N,M,T,P,D;//输入的参数，节点数量N、连边数量M、业务数量T、单边通道数量 P、最大衰减距离D
int tmp_p;
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
    //cnt1表示业务重复数，cnt2表示最短路经过的波长跳数
    int cnt1,cnt2,cnt3;
    //按cnt2从大到小，然后cnt1从小到大
    query_input(){}
    query_input(int u,int v,int id):u(u),v(v),id(id){}
    bool operator < (const query_input& A)const{
        if(cnt2==A.cnt2)return cnt1 > A.cnt1;
        return cnt2 > A.cnt2;
    }
    //按cnt2从大到小，然后cnt1从小到大:5347
    //按cnt2从大到小，然后cnt1从大到小:5313
    //按cnt4从大到小，按cnt2从大到小，然后cnt1从大到小:5319
    //按cnt3从大到小，按cnt2从大到小，然后cnt1从大到小:5347
    //波长递增：5278
    //加边，无限制版本按照广度优先求解。4892
};

struct work_answer{
    int p_id,m_cnt,n_cnt;
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
void init(){
	memset(head,-1,sizeof(head));
	cnt = 0;
}

/*
功能：把图复制一份，head->headcopy，用于最终求路由放置。
*/
void updateedge(int n,int cnt){
    for(int i = 0;i < n;i++){
        headcopy[i] = head[i];
    }
    for(int i = 0;i < cnt;i++){
        ecopy[i] = e[i];
    }
}


/*
功能：新增边，一条u->v的有向边
参数：u、v表示这条边的两个端点，w代表这条边的权值，id对应这条边在输入中的顺序
*/
void add(int u,int v,int w,int id){
	e[cnt].next = head[u];
    e[cnt].u = u;e[cnt].v = v;
	e[cnt].w = w;e[cnt].id = id;
    e[cnt].pos = cnt;head[u] = cnt++; 
}




/*
功能：给业务s-t找到一条波长为curnum的最短路，并把路径经过的边放到ans_edge里
参数：s-t表示业务的起点-终点，curnum表示当前求的波长，ans_edge表示当前求的最短路，
f为1表示求的最短路，为其他表示为最长路（随机边权）
返回值：1表示成功找到一条路径，0表示未找到一条路径
*/
int spfanormal(int s,int t,int curnum,vector<int>&ans_edge,int f){
    const int inf = 0x3f3f3f3f;
    ans_edge.clear();
    memset(dis,0x3f,sizeof(int)*N);
    memset(vis,0,sizeof(int)*N);
    dis[s] = 0;
    vis[s] = 1;
    pre[s].u = -1;
    queue<int>q;
    q.push(s);

    while(q.size()>0){
        int u = q.front();
        q.pop();
        vis[u] = 0;
        for(int i = head[u];i != -1;i = e[i].next){
            if(e[i].curp[curnum])continue;
            int w;
            if(f==1){
                w = e[i].w;
            }else w = e[i].curw;
            if(dis[e[i].v]>dis[e[i].u]+w){
                dis[e[i].v] = dis[e[i].u] + w;
                //记录前驱最短路中的节点
                pre[e[i].v].id = e[i].pos;
                pre[e[i].v].u = e[i].u;
                if(vis[e[i].v]==0){
                    q.push(e[i].v);
                    vis[e[i].v] = 1;
                }
            }
        }
    }

    if(dis[t]==inf)return 0;
    int curv = t;
    //从终点往前遍历，记录这条路径
    while(pre[curv].u!=-1){
      
        ans_edge.push_back(pre[curv].id);
   
        curv = pre[curv].u;
  
    }
    //因为是从后到前的，所以需要翻转一下
    reverse(ans_edge.begin(),ans_edge.end());
    
    return 1;
}

/*
功能：给业务s-t找到最短路，没有波长的限制，并把路径经过的边放到ans_edge里
参数：s-t表示业务的起点-终点，ans_edge表示当前求的最短路，
f为1表示求的最短路，为其他表示为最长路（随机边权）
返回值：1表示成功找到一条路径，0表示未找到一条路径，连通图应该必然存在一条路径
            初赛版本
*/
int spfa1_normal(int s,int t,vector<int>&ans_edge,int f){
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
        //assert(u>=0&&u<maxn);
        vis[u] = 0;
        for(int i = head[u];i != -1;i = e[i].next){
            int w;
            if(f==1){
                w = e[i].w;
            }else w = e[i].curw;
            w=1;
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


struct astarnode{
    int u;
    int f,g;
    vector<int>path;
    astarnode(){}
    astarnode(int u,int f,int g,const vector<int>&path):u(u),f(f),g(g),path(path){}
    bool operator < (const astarnode &A)const{
        return f > A.f;
    }
};
int h[maxn];
int k_short_path(int s,int t,int k,vector<vector<int> >&ans_edge,int limits,int f){
    vector<int>res_dege;
    if(limits==-1){//表示无限制
        int sta = spfa1_normal(t,s,res_dege,1);
        if(sta==0)return 0;
        for(int i = 0;i < N;i++){
            h[i] = dis1[i];
        }
    }
    else {
        int sta = spfanormal(t,s,limits,res_dege,1);
        if(sta==0)return 0;
        for(int i = 0;i < N;i++){
            h[i] = dis[i];
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
            set<int>sp;
            for(size_t i = 0;i < now.path.size();i++){
                sp.insert(e[now.path[i]].id);
            }
            int pre = ans_edge.size();
            if(sp.size()==now.path.size()){
                ++cnt;
                ans_edge.push_back(now.path);
            }
            //cout <<"边大小：" <<ans_edge.size() << endl;
            if(pre==ans_edge.size())cursize++;
            else cursize = 0;
            if(cursize>=50)break;
        }
        //cout << "u:" << now.u << endl;
        for(int i = head[now.u];i != -1;i = e[i].next){
            if(limits!=-1){
                if(e[i].curp[limits])continue;
            }
            int v = e[i].v;
            int g = now.g + e[i].w;
            int f = g + h[v];
            path = now.path;
            path.push_back(e[i].pos);
            q.push(astarnode(v,f,g,path));
        }
    }
    
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
void readfile(vector<edge_input> &edge,vector<query_input>&query){
    freopen("4.in","r",stdin);
    freopen("44.out","w",stdout);
    scanf("%d%d%d%d%d",&N,&M,&T,&P,&D);
    for(int i = 0;i < M;++i){
        int u,v,w;
        scanf("%d%d%d",&u,&v,&w);
        edge.push_back(edge_input(u,v,w));
        maxedgew = max(maxedgew,w);

        //记录边u-v的最小权值
        pair<int, int>tp = make_pair(min(u,v), max(v,u));
        if(mp.count(tp)){
            mp[tp] = min(mp[tp],w);
        }else mp[tp] = w;

    }
    for(int i = 0;i < T;++i){
        int u,v;
        scanf("%d %d",&u,&v);
        query.push_back(query_input(u,v,i));
    }
}


/*
功能：输出所有的输入，验证信息
参数：输入的边集合dege,输入的业务集合query
*/
void print(const vector<edge_input> &edge,const vector<query_input>&query){
    printf("%d %d %d %d %d\n",N,M,T,P,D);

    for(int i = 0;i < M;++i){
        printf("%d %d %d\n",edge[i].u,edge[i].v,edge[i].w);
    }

    for(int i = 0;i < T;++i){
        printf("%d %d %d\n",query[i].u,query[i].v,query[i].id);
    }
}

/*
功能：把输入的边构建成一个图
参数：要构建图的边集合
*/
void constructgraph(const vector<edge_input> &edge){
    //初始化图的数据结构
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
        vector<int>ans_edge;
        spfa1_normal(u,v, ans_edge,1);
        query[i].cnt1 = work[make_pair(min(u,v), max(u,v))];
        query[i].cnt2 = ans_edge.size() + 1;

        cal_conflict(ans_edge);
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

    sort(query.begin(),query.end());
}

/*
功能：撤销path里所有边波长为aimp的负载。
参数：path为需要撤回的边集合，aimp表示波长。
*/
void un_do(const vector<int>& path,int aimp){
    //撤销边的负载和波长
    for(auto i:path){
        e[i].curp[aimp]=0;//未使用
        e[i].loadnum--;
        if(i&1){
            e[i-1].curp[aimp] = 0;
            e[i-1].loadnum--;
        }else {
            e[i+1].curp[aimp] = 0;
            e[i+1].loadnum--;
        }
    }
}

/*
功能：重新分配path里所有边波长为aimp的负载，分配给q_业务。
参数：path为需要撤回的边集合，aimp表示波长，q_此次分配的业务。
*/
void _do(const vector<int>& path,int aimp,int q_){
    //分配边的负载和波长
    for(auto i:path){
        e[i].curp[aimp]=1;//使用
        e[i].loadnum++;
        if(i&1){
            e[i-1].curp[aimp] = 1;
            e[i-1].loadnum++;
        }else {
            e[i+1].curp[aimp] = 1;
            e[i+1].loadnum++;
        }
    }
}
int seed=11;//11(59356466) 1(59959677)
/*交换业务*/
bool rollback(const vector<query_input> &query,int k,vector<int>& yewu,vector<vector<int>>& work_path,vector<int>& RFL,vector<int>& pp,int flag){
    //只有相同波长的业务一起撤销才有效,不同波长互不干扰
    bool f1=false;
    int u=query[k].u;
    int v=query[k].v;
    unordered_map<int,vector<int>> rfl_p_pre;//不同波长对于的业务
    for(auto& qq:yewu){
        int rfl_p=RFL[query[qq].id];
        rfl_p_pre[rfl_p].emplace_back(qq);
    }
    for(auto& ii:rfl_p_pre){
        int rfl_p=ii.first;//该波长去重新规划业务k的路
        for(auto& qq_tmp:ii.second){
            auto tmp=work_path[query[qq_tmp].id];
            un_do(tmp,rfl_p);
        }
        //先排当前业务，用撤回的波长
        vector<int> ans_edgetmp;
        int f1_ = spfanormal(u,v,rfl_p,ans_edgetmp,flag);
        if(f1_){
            //当前波长可以有新的路径，试着将其分配，并将撤回的业务（波长相同重新分配，不同直接回滚）重新分配。
            _do(ans_edgetmp,rfl_p,k);
            bool f__=false;
            vector<vector<int>> work_path_tmp;//记录新的路径
            vector<int> RFL_tmp;
            for(auto& qq_tmp:ii.second){
                //重新分配
                vector<int> ans_edgetmp1;
                srand(seed++);
                random_shuffle(pp.begin(),pp.end());
                int uu=query[qq_tmp].u;
                int vv=query[qq_tmp].v;
                int cur_p=-1;
                for(int i=0;i<P;i++){
                    int f_=spfanormal(uu,vv,pp[i],ans_edgetmp1,flag);
                    if(f_){
                        cur_p=pp[i];
                        break;
                    }
                }
                if(cur_p==-1){
                    //该业务重新分配失败
                    //un_do(ans_edgetmp,rfl_p);
                    f__=true;
                    break;
                }
                else{
                    //当前业务成功，还要继续分配
                    work_path_tmp.emplace_back(ans_edgetmp1);
                    RFL_tmp.emplace_back(cur_p);
                    _do(ans_edgetmp1,cur_p,qq_tmp);
                }
            }
            if(f__){
                //相同波长业务分配失败，需要回滚到最初状态
                un_do(ans_edgetmp,rfl_p);
                for(int i=0;i<work_path_tmp.size();i++){
                    //对相关波长业务，进行回滚
                    un_do(work_path_tmp[i],RFL_tmp[i]);
                }
                for(auto& qq_tmp:ii.second){
                    auto tmp=work_path[query[qq_tmp].id];
                    _do(tmp,rfl_p,qq_tmp);
                }
                //next bochang
            }
            else{
                //this bochang重新分配成功
                f1=true;
                for(int i=0;i<work_path_tmp.size();i++){
                    RFL[query[ii.second[i]].id]=RFL_tmp[i];
                    work_path[query[ii.second[i]].id]=work_path_tmp[i];
                }
                RFL[query[k].id]=rfl_p;
                work_path[query[k].id]=ans_edgetmp;
                return f1;
            }
        }
        else{
            for(auto& qq_tmp:ii.second){
                auto tmp=work_path[query[qq_tmp].id];
                _do(tmp,rfl_p,qq_tmp);
            }
        }
    
    }

    return f1;
}


/*
使用前提：图必须要建好、业务已经排好序
功能：求解每条业务的答案
参数：
*/

void solution_normal(const vector<edge_input> &edge,const vector<query_input> &query,
    vector<addedge> &add_edge,vector<vector<int> > &work_path,vector<int> &RFL){
    
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
        flag = rand()%2;
        bool f = 0;
        vector<int>ans_edge;
        vector<vector<int>>ans_edges;
        bool multi_path=false;
        int curp = -1;
        vector<int> pp(P);
        for(int i = 0;i < P;i++){
            pp[i] = i;
        }
        srand(seed++);
        random_shuffle(pp.begin(), pp.end());
        for(int i = 0;i < P;i++){
            //f = spfanormal(u,v,pp[i],ans_edge,flag);
            f = k_short_path(u,v,10,ans_edges,pp[i],flag);
             multi_path=true;
            curp = pp[i];
            if(f)break;
        }
        if(!(f&&curp!=-1)){
            flag = !flag;
            for(int i = 0;i < P;i++){
                //f = spfanormal(u,v,pp[i],ans_edge,flag);
                f = k_short_path(u,v,10,ans_edges,pp[i],flag);
                multi_path=true;
                curp = pp[i];
                if(f)break;
            }
        }
        //尝试不加边
        //srand(seed++);
        if(!(f&&curp!=-1)){
            // bool f1=false;
            // vector<int> qq_tmp(k-1);
            // for(int i=0;i<k-1;i++){
            //     qq_tmp[i]=i;
            // }
            // srand(seed++);
            // random_shuffle(qq_tmp.begin(), qq_tmp.end());
            // int len_tmp=qq_tmp.size();
            // for(int qqq=0;qqq<len_tmp;qqq++){
            //     vector<int> yewu_tmp{qq_tmp[qqq]};
            //     f1=rollback(query,k,yewu_tmp,work_path,RFL,pp,flag);
            //     //cout<<f1<<endl;
            //     if(f1){
            //         break;
            //     }
            // }
            // if(f1){
            //     continue;
            // }
            // for(int qqq=0;qqq<len_tmp;qqq++){
            //     for(int qqqq=qqq+1;qqqq<len_tmp;qqqq++){
            //         if(RFL[query[qq_tmp[qqq]].id]!=RFL[query[qq_tmp[qqqq]].id]){
            //             continue;
            //         }
            //         vector<int> yewu_tmp{qq_tmp[qqq],qq_tmp[qqqq]};
            //         f1=rollback(query,k,yewu_tmp,work_path,RFL,pp,flag);
            //         if(f1){
            //             break;
            //         }
            //     }
            //     if(f1){
            //         break;
            //     }
            // }
            // if(f1){
            //     continue;
            // }
            /*撤回单个业务*/
            bool f1=false;
            vector<int> qq_tmp;
            for(int i=0;i<=k;i++){
                qq_tmp.push_back(i);
            }
            srand(seed++);
            random_shuffle(qq_tmp.begin(), qq_tmp.end());
            int len_tmp=qq_tmp.size();
            for(int qqq=0;qqq<len_tmp;qqq++){
                int qq=qq_tmp[qqq];
                auto tmp=work_path[query[qq].id];
                int rfl_p=RFL[query[qq].id];
                //交换两个业务的规划路线，先撤销原有业务的边--，再规划当前业务，再规划原有业务            
                un_do(tmp, rfl_p);
                vector<int> ans_edgetmp;
                int f_ = spfanormal(u,v,rfl_p,ans_edgetmp,flag);
                if(f_){
                    //试试规划原来的业务，所有波长,在此之前需要将k业务规划占用波长和负载++
                    _do(ans_edgetmp,rfl_p,k);
                    vector<int> ans_edgetmp1;
                    srand(seed++);
                    random_shuffle(pp.begin(), pp.end());
                    int uu=query[qq].u;
                    int vv=query[qq].v;
                    int cur_p=-1;
                    for(int i = 0;i < P;i++){
                        int f__ = spfanormal(uu,vv,pp[i],ans_edgetmp1,flag);
                        if(f__){
                            cur_p=pp[i];
                            break;
                        }
                    }
                    if(cur_p==-1){
                        //重新分配qq业务的
                        un_do(ans_edgetmp,rfl_p);//加边
                        _do(tmp,rfl_p,qq);
                    }
                    else{
                        //成功，先将qq新规划的路径++
                        _do(ans_edgetmp1,cur_p,qq);
                        //还需要跟新k和qq业务的全局变量
                        RFL[query[qq].id]=cur_p;
                        work_path[query[qq].id]=ans_edgetmp1;
                        RFL[query[k].id]=rfl_p;
                        work_path[query[k].id]=ans_edgetmp;
                        //告诉下面已经分配
                        f1=true;
                        break;
                    }
                }
                else{
                    //无效，返回,路径和波长都不变
                    _do(tmp,rfl_p,qq);
                }
            }
            if(f1){
                continue;
            }
        }
        if(f&&curp!=-1){
            if(multi_path){
                int mincou = 0;
                //选择跳数最少得路径
                for(size_t i = 0;i < ans_edges.size();i++){
                    if(ans_edges[mincou].size()>ans_edges[i].size()){
                        mincou = i;
                    }
                }
                ans_edge = ans_edges[mincou];
            }
            RFL[query[k].id]=curp;
            _do(ans_edge,curp,k);
            work_path[query[k].id]=ans_edge;
        }else{
            if(P<tmp_p){
                P++;
                k--;
                continue;
            }
            vector<int>nolimitans_edge;
            spfa1_normal(u, v, nolimitans_edge,flag);
            int addedgemin = -1;
            vector<addedge>nowadd;
            int maxloadnumid = 0;
            for(int i = 0;i < P;i++){
                f = spfanormal(u,v,i,work_path[query[k].id],flag);
                vector<addedge>curadd;
                int loadnum = 0;
                int curmaxloadnumid = 0;
                int curmaxloadnum = -1;
                for(int ie = 0;ie < nolimitans_edge.size();ie++){
                    int uu = e[nolimitans_edge[ie]].u;
                    int vv = e[nolimitans_edge[ie]].v;
                    if(dis[uu]!=inf&&dis1[vv]!=inf&&dis[vv]==inf){
                        curadd.push_back(addedge(uu,vv));
                        if(e[nolimitans_edge[ie]].loadnum>curmaxloadnum){
                            curmaxloadnumid = curadd.size() - 1;
                            curmaxloadnum = e[nolimitans_edge[ie]].loadnum;
                        }
                        loadnum += e[nolimitans_edge[ie]].loadnum;
                        
                    }
                }
                if(loadnum>addedgemin){
                    addedgemin = loadnum;
                    nowadd = curadd;
                    maxloadnumid = curmaxloadnumid;
                } 
            }
            int uu,vv;
            uu = nowadd[maxloadnumid].u;
            vv = nowadd[maxloadnumid].v;
            add(uu,vv,mp[make_pair(min(uu,vv), max(uu,vv))],curm);
                // curm++;
            add(vv,uu,mp[make_pair(min(uu,vv), max(uu,vv))],curm);
            e[cnt-1].curw = maxedgew - e[cnt-1].w;
            e[cnt-2].curw = maxedgew - e[cnt-2].w;
            curm++;
            add_edge.push_back(addedge(uu,vv));
            k--;
        }
    }

}


/*
功能：输出最终的答案
参数：新增的边，最终每条业务的答案
*/
void printanswer(const vector<addedge> & add_edge,const vector<work_answer>& ans){
    //输出新增的边
    printf("%d\n",(int)add_edge.size());
    for(size_t i = 0;i < add_edge.size();++i){
        printf("%d %d\n",add_edge[i].u,add_edge[i].v);
    }
    //输出每条业务的答案
    for(size_t i = 0;i < ans.size();++i){
        printf("%d %d %d ",ans[i].p_id,ans[i].m_cnt,ans[i].n_cnt);
        for(size_t j = 0;j < ans[i].ans_edge_id.size();++j){
            printf("%d ",ans[i].ans_edge_id[j]);
        }
        for(size_t j = 0;j < ans[i].ans_router.size();++j){
            printf("%d ",ans[i].ans_router[j]);
        }
        printf("\n");
    }
}


int main(){
    int st = clock();
    int ed;
    vector<edge_input>edge_input;//存储输入的边 u-v表示边、w表示边权
    vector<query_input>query_input;//存储输入的查询 u-v表示边、id表示输入的顺序   
    //读取输入
    readfile(edge_input,query_input);
    //构建图
    constructgraph(edge_input);
    //业务排序
    calworkorders(query_input);
    // for(int i = 0;i < T;++i){
    //     printf("%d %d %d\n",query_input[i].u,query_input[i].v,query_input[i].id);
    // }
    vector<addedge> max_add_edge;
    vector<vector<int> > work_path_max;
    vector<int> RFL_max;
    int max_add=INT_MAX;
    tmp_p=P;
    while(true){
        P=ceil(tmp_p/1.5);
        vector<addedge>add_edge;
        vector<vector<int> > work_path;
        vector<int> RFL;
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
        ed = clock();
        if(double(ed-st)/CLOCKS_PER_SEC>0)break;
    }
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
    printanswer(max_add_edge,ans);
    return 0;
}