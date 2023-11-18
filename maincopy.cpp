#include<bits/stdc++.h>
#include <cassert>
#include <climits>
#include <cstdint>
#include <utility>
#include <vector>
using namespace std;
typedef long long ll;
//maxm表示边的最大范围（包括新加进来的边）
const int maxm = 2e5+10;
//maxn表示点的最大范围
const int maxn = 5010;
// 题目的参数
int n,m,t,p,d,r;
//存图的结构
int head[maxn];
int headcopy[maxn];
struct edge{
    int u,v,w,id,curp[85],next,pos;
    bool connect = true;
    int loadnum,curw;
    int cost = 10;
};
const int inf = 0x3f3f3f3f;
edge e[maxm<<1];//原始的边集合
edge ecopy[maxm<<1];//while循环里，目前为止最优的边集合。

struct node{
    int u,id;
};
struct addedge{//记录加边的两个端点
    int u,v;
};
//记录全局每条边的每个波长分配的业务对象？
int Curp[maxm<<1][85];

int maxedge = 0;//记录输入里面最大的边权，用于求最长路的时候每条边的路径权值
int cnt;//记录边的个数，边是成对出现0和1、2和3

/*
功能：初始化边节点，清空所有边。
*/
void init(){
	memset(head,-1,sizeof(head));
	cnt = 0;
}

/*
功能：新增边，一条u->v的有向边
参数：u、v表示这条边的两个端点，w代表这条边的权值，id对应这条边在输入中的顺序
*/
void add(int u,int v,int w,int id){
	e[cnt].next = head[u];
    e[cnt].u = u;e[cnt].v = v;
	e[cnt].w = w;e[cnt].id = id;
    e[cnt].connect = true;
    e[cnt].pos = cnt;head[u] = cnt++; 
}

void myprint(vector<int>& T){
    for(auto& i:T){
        cout<<i<<" ";
    }
    cout<<endl;
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
        Curp[i][aimp]=0;
        if(i&1){
            e[i-1].curp[aimp] = 0;
            e[i-1].loadnum--;
            Curp[i-1][aimp]=0;
        }else {
            e[i+1].curp[aimp] = 0;
            e[i+1].loadnum--;
            Curp[i+1][aimp]=0;
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
        Curp[i][aimp]=q_;
        if(i&1){
            e[i-1].curp[aimp] = 1;
            e[i-1].loadnum++;
            Curp[i-1][aimp]=q_;
        }else {
            e[i+1].curp[aimp] = 1;
            e[i+1].loadnum++;
            Curp[i+1][aimp]=q_;
        }
    }
}

//记录最短路中的路径
node pre[maxn];

int vis[maxn];
int dis[maxn];

/*
功能：给业务s-t找到一条波长为curnum的最短路，并把路径经过的边放到ans_edge里
参数：s-t表示业务的起点-终点，curnum表示当前求的波长，ans_edge表示当前求的最短路，
f为1表示求的最短路，为其他表示为最长路（随机边权）
返回值：1表示成功找到一条路径，0表示未找到一条路径
*/
int spfa(int s,int t,int curnum,vector<int>&ans_edge,int f){
    const int inf = 0x3f3f3f3f;
    ans_edge.clear();
    memset(dis,0x3f,sizeof(int)*n);
    memset(vis,0,sizeof(int)*n);
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
            if(e[i].connect==false||e[i].curp[curnum])continue;
            int w=1;

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
int dis1[maxn];
/*
功能：给业务s-t找到最短路，没有波长的限制，并把路径经过的边放到ans_edge里
参数：s-t表示业务的起点-终点，ans_edge表示当前求的最短路，
f为1表示求的最短路，为其他表示为最长路（随机边权）
返回值：1表示成功找到一条路径，0表示未找到一条路径，连通图应该必然存在一条路径
*/
int spfa1(int s,int t,vector<int>&ans_edge,int f){
    ans_edge.clear();
    memset(dis1,0x3f,sizeof(int)*n);
    memset(vis,0,sizeof(int)*n);
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
            // if(e[i].connect==false)continue;
            int w;
            if(f==1){
                w = e[i].w;
            }else w = e[i].curw;
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
        if(sum>d){
            sum = e[ans_edge[i]].w;
            ans[0].push_back(e[ans_edge[i]].u);
        }
    }
    sum = 0;
    for(int i = ans_edge.size() - 1;i >= 0;i--){
        sum += e[ans_edge[i]].w;
        if(sum>d){
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
        if(sum>d){
            sum = ecopy[ans_edge[i]].w;
            ans[0].push_back(ecopy[ans_edge[i]].u);
        }
    }
    sum = 0;
    for(int i = ans_edge.size() - 1;i >= 0;i--){
        sum += ecopy[ans_edge[i]].w;
        if(sum>d){
            sum = ecopy[ans_edge[i]].w;
            ans[1].push_back(ecopy[ans_edge[i]].v);
        }
    }
    if(ans[0].size()<ans[1].size())bignum = ans[0];
    else bignum = ans[1];
}

//对查询排序的一个结构体
struct query{
    int u,v,pathcnt,id;
    //cnt1表示业务重复数，cnt2表示最短路经过的波长跳数
    int cnt1,cnt2,cnt3;
    //按cnt2从大到小，然后cnt1从小到大
    bool operator < (const query& A)const{
        if(cnt2==A.cnt2)return cnt1 < A.cnt1;
        return cnt2 > A.cnt2;
    }
};
query q[10010];
//vector<vector<int>> final;
int edge_input[maxm][5];//记录输入的边集合
int q_input[10000][5];//记录输入的查询集合

// unorder_map< pair<int, int>, int>mp;//记录边u-v的最小权值
int mp[maxn][maxn];
/*
功能：重新初始化边，重新构建原始图
*/
void re_init(){
    memset(e,0,sizeof(e));
    memset(head,-1,sizeof(head));
	cnt = 0;
    for(int i=0;i<m;i++){
        int u,v,w;
        u=edge_input[i][0];
        v=edge_input[i][1];
        w=edge_input[i][2];
        add(u,v,w,i);
        add(v,u,w,i);
    } 
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

void pausedo(const vector<int>&path){
    for(auto i:path){
        e[i].connect = false;
        if(i&1){
            e[i-1].connect = false;
        }else {
            e[i+1].connect = false;
        }
    }
}

void pauseundo(const vector<int>&path){
    for(auto i:path){
        e[i].connect = true;
        if(i&1){
            e[i-1].connect = true;
        }else {
            e[i+1].connect = true;
        }
    }
}

vector<bitset<5000> >conflict;
void cal_conflict(const vector<int> &nolimitans_edge){
    bitset<5000>edge_conflict;
    for(int i = 0;i < nolimitans_edge.size();i++){
        edge_conflict[e[nolimitans_edge[i]].id] = 1;
    }
    conflict.push_back(edge_conflict);
}

void test1(){
    // freopen("../4.in_muti_1","r",stdin);
    // freopen("../4.out_muti","w",stdout);
     freopen("1.in","r",stdin);
     freopen("1.out","w",stdout);
    int seed=11;//11(59356466) 1(59959677)
    clock_t st = clock();
    clock_t et;
    memset(mp,0x3f,sizeof(mp));
    scanf("%d%d%d%d%d%d",&n,&m,&t,&r,&p,&d);
    init();
    for(int i = 0;i < m;i++){
        int u,v,w;
        scanf("%d%d%d",&u,&v,&w);
        edge_input[i][0]=u;
        edge_input[i][1]=v;
        edge_input[i][2]=w;
        maxedge = max(maxedge,w);//最大的边权
        add(u,v,w,i);
        add(v,u,w,i);

        //记录边u-v的最小权值
        pair<int, int>tp = make_pair(min(u,v), max(v,u));
        mp[min(u,v)][max(u,v)] = min(mp[min(u,v)][max(u,v)],w);

    }
    // pair<int, int>tp = make_pair(25,30);
    // cout << "mp:"<<mp[tp] << endl;
    //得到curw，表示边权的相对权值，可以用作求最长路的边权
    for(int i = 0;i < cnt;i++){
        e[i].curw = maxedge - e[i].w;
    }

    map<pair<int, int> ,int> yewu;//业务重复次数

    //输入业务，记录重复次数
    for(int i = 1;i <= t;i++){
        int u,v,pathcnt;
        scanf("%d%d%d",&u,&v,&pathcnt);
        q_input[i][0]=u;
        q_input[i][1]=v;
        q_input[i][2]=i;
        q_input[i][3] = pathcnt;
        yewu[make_pair(min(u,v), max(u,v))]++;
        q[i].u = u;
        q[i].v = v;
        q[i].pathcnt = pathcnt;
        q[i].id = i;
    }
    // cout << "读入完成\n";
    conflict.clear();
    //求解业务的排序参数cnt1、cnt2
    for(int i = 1;i <= t;i++){
        int u,v;
        u = q[i].u;
        v = q[i].v;
        vector<int>nolimitans_edge;
        spfa1(u, v, nolimitans_edge,1);
        q[i].cnt2 = nolimitans_edge.size() + 1;
        q[i].cnt1 = yewu[make_pair(min(u,v), max(u,v))];
        cal_conflict(nolimitans_edge);
    }
    // for(int i = 0;i < conflict.size();i++){
    //     int cnt = 0;
    //     for(int j = 0;j < conflict.size();j++){
    //         if(i==j)continue;
    //         if((conflict[i]&conflict[j])!=0)cnt++;
    //     }
    //     q[i+1].cnt3 = cnt;
    // }
    
    sort(q+1,q+1+t);
    vector<addedge> max_add_edge;
    vector<vector<vector<int> > > work_path_max;
    vector<vector<int> >  RFL_max;
    vector<vector<vector<int> > > big_max(t+1);
    int max_add=INT_MAX;
    while(true){
        vector<addedge>add_edge;
        vector<vector<vector<int> > > work_path(t+1);
        vector<vector<vector<int> > > big(t+1);
        vector<vector<int> > RFL(t+1);
        re_init();
        int curm = m;
        int flag = 1;
        for(int k = 1;k <= t;k++){
            cout << "当前处理的业务:" << q[k].id << " " << q[k].pathcnt << endl;
            int u = q[k].u;
            int v = q[k].v;
            const int pathcnt = q[k].pathcnt;
            vector<vector<int> >pause_edges;
            vector<vector<int > >pausebigtmp;
            vector<int > pauseRFL;
            if(q[k].pathcnt==2){
                bool f = 0;
                vector<int>ans_edge1,ans_edge2;
                int curp = -1;
                vector<int>  pp(p);
                for(int i = 0;i < p;i++){
                    pp[i] = i;
                }
                srand(seed++);
                random_shuffle(pp.begin(), pp.end());
                bool ff = 0;
                for(int i = 0;i < p;i++){
                    f = spfa(u,v,pp[i],ans_edge1,flag);
                    pausedo(ans_edge1);
                    ff = spfa(u,v,pp[i],ans_edge2,flag);
                    curp = pp[i];
                    if(f&&ff)break;
                    pauseundo(ans_edge1);
                }
                // if(!(f&&curp!=-1)){
                //     flag = !flag;
                //     for(int i = 0;i < p;i++){
                //         f = spfa(u,v,pp[i],ans_edge,flag);
                //         curp = pp[i];
                //         if(f)break;
                //     }
                // }
                if(!f||!ff){
                    pauseundo(ans_edge1);
                }
                if(f&&ff){
                    vector<int>bigtmp;
                    // getbig(ans_edge, bigtmp);
                    _do(ans_edge1,curp,k);
                    _do(ans_edge2,curp,k);
                    pausedo(ans_edge1);
                    pausedo(ans_edge2);
                    
                    pauseRFL.push_back(curp);
                    pauseRFL.push_back(curp);
                    pause_edges.push_back(ans_edge1);
                    pause_edges.push_back(ans_edge2);
                    assert(pathcnt==pause_edges.size());
                    // big[q[k].id]=bigtmp;
                    
                    RFL[q[k].id]=pauseRFL;
                    work_path[q[k].id]=pause_edges;
                    
                    // for(int kkk = 0;kkk < pause_edges.size();kkk++){
                    //     myprint(pause_edges[kkk]);
                    // }
                    for(int Pcnt = 0;Pcnt < pathcnt;Pcnt++){
                        pauseundo(pause_edges[Pcnt]);
                    }
                }else{
                    vector<int>nolimitans_edge;
                    vector<int>limitans_edge;
                    spfa1(u, v, nolimitans_edge,flag);
                    int addedgemin = -1;
                    vector<addedge>nowadd;
                    cout << "加边" << endl;
                    for(int i = 0;i < nolimitans_edge.size();i++){
                        int uu = e[nolimitans_edge[i]].u,vv = e[nolimitans_edge[i]].v;
                        if(dis1[vv]!=inf&&dis[vv]==inf){
                            add(uu,vv,mp[min(u,v)][max(u,v)],curm);
                            add(vv,uu,mp[min(u,v)][max(u,v)],curm);
                            curm++;
                            // nowadd.push_back(addedge{uu,vv});
                            add_edge.push_back(addedge{uu,vv});
                        }
                    }
                    int maxloadnumid = 0;
                    // for(int i = 0;i < p;i++){
                    //     f = spfa(u,v,i,limitans_edge,flag);
                    //     vector<addedge>curadd;
                    //     int loadnum = 0;
                    //     int curmaxloadnumid = 0;
                    //     int curmaxloadnum = -1;
                    //     for(int ie = 0;ie < nolimitans_edge.size();ie++){
                    //         int uu = e[nolimitans_edge[ie]].u;
                    //         int vv = e[nolimitans_edge[ie]].v;
                    //         if(dis[uu]!=inf&&dis1[vv]!=inf&&dis[vv]==inf){
                                
                    //             curadd.push_back(addedge{uu,vv});
                    //             if(e[nolimitans_edge[ie]].loadnum>curmaxloadnum){
                    //                 curmaxloadnumid = curadd.size() - 1;
                    //                 curmaxloadnum = e[nolimitans_edge[ie]].loadnum;
                    //             }
                    //             loadnum += e[nolimitans_edge[ie]].loadnum;
                                
                    //         }
                    //     }
                        
                    //     if(loadnum>addedgemin){
                    //         addedgemin = loadnum;
                    //         nowadd = curadd;
                    //         maxloadnumid = curmaxloadnumid;
                    //     } 
                    // }
                    // int uu,vv;
                    // uu = nowadd[maxloadnumid].u;
                    // vv = nowadd[maxloadnumid].v;
                    // add(uu,vv,mp[make_pair(min(uu,vv), max(uu,vv))],curm);
                    //     // curm++;
                    // add(vv,uu,mp[make_pair(min(uu,vv), max(uu,vv))],curm);
                    // e[cnt-1].curw = maxedge - e[cnt-1].w;
                    // e[cnt-2].curw = maxedge - e[cnt-2].w;
                    // curm++;
                    // add_edge.push_back(addedge{uu,vv});
                    // k--;
                    // Pcnt--;

                }

            }else {
                //大于等于3
                // cout << 12 << endl;
                for(int Pcnt = 0;Pcnt < pathcnt;Pcnt++){
                    // flag = rand()%2;
                    bool f = 0;
                    vector<int>ans_edge;
                    int curp = -1;
                    vector<int>  pp(p);
                    for(int i = 0;i < p;i++){
                        pp[i] = i;
                    }
                    
                    srand(seed++);
                    random_shuffle(pp.begin(), pp.end());
                    for(int i = 0;i < p;i++){
                        f = spfa(u,v,pp[i],ans_edge,flag);
                        curp = pp[i];
                        if(f)break;
                    }
                    cout<<f<<endl;
                    //cout << Pcnt << endl;
                    if(f&&curp!=-1){
                     //   cout << "12313-1" << endl;
                        // getbig(ans_edge, bigtmp);
                     //   cout << Pcnt << endl;
                        _do(ans_edge,curp,k);
                        pausedo(ans_edge);
                        pauseRFL.push_back(curp);
                        pause_edges.push_back(ans_edge);
                    }else{
                       // cout << "12313-2" << endl;
                        // pair<int, int>tp = make_pair(25,30);
                        // cout << mp[tp] << endl;
                        cout << "加边" << endl;
                        vector<int>nolimitans_edge;
                        vector<int>limitans_edge;
                        spfa1(u, v, nolimitans_edge,flag);
                    
                       // cout << nolimitans_edge.size() << endl;
                        int addedgemin = -1;
                        for(int i = 0;i < nolimitans_edge.size();i++){
                            
                            int uu = e[nolimitans_edge[i]].u,vv = e[nolimitans_edge[i]].v;
                            // cout << min(uu,vv)<<  max(uu,vv) << endl;
                            add(uu,vv,mp[min(u,v)][max(u,v)],curm);
                            add(vv,uu,mp[min(u,v)][max(u,v)],curm);
                            cout<<uu<<" "<<vv<<" "<<curm<<endl;
                            
                            curm++;
                            // nowadd.push_back(addedge{uu,vv});
                            add_edge.push_back(addedge{uu,vv});
                            //cout << (dis[uu]!=inf&&dis1[vv]!=inf&&dis[vv]==inf) << endl;
                            // if(dis[uu]!=inf&&dis1[vv]!=inf&&dis[vv]==inf){
                            //     cout << min(uu,vv)<<  max(uu,vv) << endl;
                            //     // pair<int, int>tp = make_pair(25,30);
                            //     // cout << mp[tp] << endl;
                            //     //cout << mp[make_pair(min(uu,vv), max(uu,vv))] << endl;
                            //     add(uu,vv,mp[min(u,v)][max(u,v)],curm);
                            //     add(vv,uu,mp[min(u,v)][max(u,v)],curm);
                            //     curm++;
                            //     // nowadd.push_back(addedge{uu,vv});
                            //     add_edge.push_back(addedge{uu,vv});
                            // }
                           // cout << uu << " " << vv << endl;
                        }
                        // int maxloadnumid = 0;
                        // for(int i = 0;i < p;i++){
                        //     f = spfa(u,v,i,limitans_edge,flag);
                        //     vector<addedge>curadd;
                        //     int loadnum = 0;
                        //     int curmaxloadnumid = 0;
                        //     int curmaxloadnum = -1;
                        //     for(int ie = 0;ie < nolimitans_edge.size();ie++){
                        //         int uu = e[nolimitans_edge[ie]].u;
                        //         int vv = e[nolimitans_edge[ie]].v;
                        //         if(dis[uu]!=inf&&dis1[vv]!=inf&&dis[vv]==inf){
                                    
                        //             curadd.push_back(addedge{uu,vv});
                        //             if(e[nolimitans_edge[ie]].loadnum>curmaxloadnum){
                        //                 curmaxloadnumid = curadd.size() - 1;
                        //                 curmaxloadnum = e[nolimitans_edge[ie]].loadnum;
                        //             }
                        //             loadnum += e[nolimitans_edge[ie]].loadnum;
                                    
                        //         }
                        //     }
                            
                        //     if(loadnum>addedgemin){
                        //         addedgemin = loadnum;
                        //         nowadd = curadd;
                        //         maxloadnumid = curmaxloadnumid;
                        //     } 
                        // }
                        // int uu,vv;
                        // uu = nowadd[maxloadnumid].u;
                        // vv = nowadd[maxloadnumid].v;
                        // add(uu,vv,mp[make_pair(min(uu,vv), max(uu,vv))],curm);
                        //     // curm++;
                        // add(vv,uu,mp[make_pair(min(uu,vv), max(uu,vv))],curm);
                        // e[cnt-1].curw = maxedge - e[cnt-1].w;
                        // e[cnt-2].curw = maxedge - e[cnt-2].w;
                        // curm++;
                        // add_edge.push_back(addedge{uu,vv});
                        // k--;
                        Pcnt--;
                    }
                   // cout << Pcnt << endl;

                }
                //cout << 12 << endl;
                assert(pathcnt==pause_edges.size());
                // big[q[k].id]=bigtmp;
                RFL[q[k].id]=pauseRFL;
                work_path[q[k].id]=pause_edges;
                for(int Pcnt = 0;Pcnt < pathcnt;Pcnt++){
                    pauseundo(pause_edges[Pcnt]);
                }
            }
            // if(pathcnt!=pause_edges.size()){
            //     cout << q[k].id << " " << pause_edges.size() << " " << q[k].pathcnt << endl;
            // }
            
        }
    
        if((int)add_edge.size()<max_add){
            max_add_edge.clear();
            max_add_edge=add_edge;
            work_path_max.clear();
            work_path_max=work_path;
            max_add=(int)add_edge.size();
            RFL_max.clear();
            RFL_max=RFL;
            big_max.clear();
            big_max=big;
            updateedge(n,cnt);
        }
        et = clock();
        //  cout << "curm:" << curm << endl;
        if(double(et-st)/CLOCKS_PER_SEC>0){
            break;
        }
        
    }
    // cout << "处理结束!" << endl;
   
    //et = clock();
   // cout<<double(et-st)/CLOCKS_PER_SEC<<endl;
    vector<vector<vector<int> > > final(t+1);
    for(int k=1;k<=t;k++){
        vector<vector<int> > fin(q[k].pathcnt); 
        big_max[q[k].id].resize(q[k].pathcnt);
        for(int Pcnt = 0;Pcnt < q[k].pathcnt;Pcnt++){
            fin[Pcnt].push_back(RFL_max[q[k].id][Pcnt]);
            fin[Pcnt].push_back((int)work_path_max[q[k].id][Pcnt].size());
            
            getbigcopy(work_path_max[q[k].id][Pcnt],big_max[q[k].id][Pcnt]);
            // myprint(big_max[q[k].id][Pcnt]);
            fin[Pcnt].push_back((int)big_max[q[k].id][Pcnt].size());
            for(int i=0;i<work_path_max[q[k].id][Pcnt].size();i++){
                fin[Pcnt].push_back(e[work_path_max[q[k].id][Pcnt][i]].id);
            }
            for(int i = 0;i < big_max[q[k].id][Pcnt].size();i++){
                fin[Pcnt].push_back(big_max[q[k].id][Pcnt][i]);
            }
            // myprint(fin[Pcnt]);
        }
        
        final[q[k].id] = fin;
    }

    // cout << "1111" << endl;
    printf("%d\n",(int)max_add_edge.size());
    for(int i = 0;i < max_add_edge.size();i++){
        printf("%d %d\n",max_add_edge[i].u,max_add_edge[i].v);
    }

    for(int i = 1;i <= t;i++){
        for(int j = 0;j < final[i].size();j++){
            for(int k = 0;k < final[i][j].size();k++){
                printf("%d ",final[i][j][k]);
            }
            printf("\n");
        }
        
    }
}



void localsearch1(){

}

int main(){
    test1();
   // test2();
  // test3();
}
/*
7 10 5 6 4 6 
0 1 5
0 2 3
1 3 1
1 4 2
2 3 5 
2 3 4 
2 5 6 
3 6 3
5 6 2
5 6 5
0 6 1
2 4 1
2 4 1
2 4 2
2 4 1
*/