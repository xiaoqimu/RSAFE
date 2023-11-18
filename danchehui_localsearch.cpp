#pragma GCC optimize(3,"Ofast","inline")
#pragma GCC optimize(2)
#include<bits/stdc++.h>
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
int n,m,t,p,d,curm;
//存图的结构
int head[maxn];
int headcopy[maxn];
struct edge{
    int u,v,w,id,curp[85],next,pos;
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
    e[cnt].pos = cnt;head[u] = cnt++; 
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
    int u,v,id;
    //cnt1表示业务重复数，cnt2表示最短路经过的波长跳数
    int cnt1,cnt2;
    //按cnt2从大到小，然后cnt1从小到大:5347
    int cnt3;
    //业务的冲突次数
    bool operator < (const query& A)const{
        if(cnt3==A.cnt3){
            if(cnt2==A.cnt2)return cnt1 > A.cnt1;
            return cnt2 > A.cnt2;
        }
        else{
            return cnt3>A.cnt3;
        }
        
    }
    //按cnt2从大到小，然后cnt1从大到小:5313
    
};
query q[10010];
//vector<vector<int>> final;
int edge_input[maxm][3];//记录输入的边集合
int q_input[10000][3];//记录输入的查询集合
int edge_num[1000000];
map< pair<int, int>, int>mp;//记录边u-v的最小权值

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
clock_t st,et;
void localsearch1(vector<vector<int>>& work_path_max,vector<int>& RFL_max,int& seed){
    //边小到大循环,Curp边的波长分配给什么业务。新边2m-2curm
    vector<int> pp;
    for(int i=0;i<p;i++){
        pp.emplace_back(i);
    }
    int l=2000;
    vector<bool> visit(2*curm,false);
    while(l--){
    // et = clock();
    // if(double(et-st)/CLOCKS_PER_SEC>40){
    //     break;
    // }
    vector<pair<int,int>> num;//业务数量，和边的下标
    vector<int> rp;
    
    for(int i=2*m,jj=0;i<2*curm;i+=2,jj++){
        //对于一条边有两边，边的每一个波长使用情况。
        int tmp=0;
        for(int j=0;j<p;j++){
            if(Curp[i][j]!=0){
                tmp+=1;
            }
        }
        num.emplace_back(tmp,i);
        rp.emplace_back(jj);
    }
    sort(num.begin(),num.end(),[](const pair<int,int>& a,const pair<int,int>& b){return a.first<b.first;});
    srand(seed++);
    random_shuffle(rp.begin(),rp.end());
    for(int i=0;i<rp.size();i++){
        int j=rp[i];
        auto tmp=num[j];
        if(tmp.first==0||visit[tmp.second]==true){
            continue;
        }
        visit[tmp.second]=true;
        int index_e=tmp.second;
        //cout<<index_e<<endl;
        bool f=true;
        vector<pair<int,int>> yewu;
        for(int k=0;k<p;k++){
            if(Curp[index_e][k]!=0){
                //记录所有业务
                yewu.emplace_back(Curp[index_e][k],k);
                //先撤销
                un_do(work_path_max[q[Curp[index_e][k]].id],k);//撤销路径的波长置为未使用
                
            }
        }
        //禁边,所有波长不可用
        for(int j=0;j<p;j++){
            e[index_e].curp[j]=1;
            e[index_e+1].curp[j]=1;
            Curp[index_e][j]=0;
            Curp[index_e+1][j]=0;
        }
        //重分配，
        vector<int> index_qq;
        vector<vector<int>> work_path_tmp;
        vector<int> RFL_tmp;
        for(auto k:yewu){
            int qq=k.first;
            int u=q[qq].u;
            int v=q[qq].v;
            vector<int> ans_edge;
            int flag=1;
            //在所有波长中寻找新的路径
            srand(seed++);
            random_shuffle(pp.begin(),pp.end());
            int curp=-1;
            for(int j_p=0;j_p<p;j_p++){
                int f1=spfa(u,v,pp[j_p],ans_edge,flag);
                if(f1){
                    curp=pp[j_p];
                    break;
                }
            }
            if(curp==-1){
                //没有新的路径,整个边上的业务不能转移
                f= false;
                break;
            }
            else{
                //继续规划
                _do(ans_edge,curp,qq);//将该业务规划出去
                work_path_tmp.emplace_back(ans_edge);
                RFL_tmp.emplace_back(curp);
                index_qq.emplace_back(qq);
            }

        }
        //如果全部成功，才真正减边，否则还原
        if(f){
            //标记该边
            for(int j=0;j<work_path_tmp.size();j++){
                // cout<<q[yewu[j].first].id<<"--"<<q[index_qq[j]].id<<endl;
                
                // for(auto jj:work_path_max[q[index_qq[j]].id]){
                //     cout<<e[jj].id<<" ";
                // }
                // cout<<endl;
                // cout<<RFL_max[q[index_qq[j]].id]<<endl;
                // cout<<"---------------"<<endl;
                // for(auto jj:work_path_tmp[j]){
                //     cout<<e[jj].id<<" ";
                // }
                // cout<<endl;
                // cout<<RFL_tmp[j]<<endl;
                work_path_max[q[index_qq[j]].id]=work_path_tmp[j];
                RFL_max[q[index_qq[j]].id]=RFL_tmp[j];
            }
           // cout<<index_e<<endl;
            edge_num[index_e]=1;
            edge_num[index_e+1]=1;
            cout<<1<<endl;
           // cout<<1<<endl;
        }
        else{
            //回滚:当前边上所有任务的撤销，再撤销新分配的部分路径,还原边
            for(int j=0;j<p;j++){
                e[index_e].curp[j]=0;
                e[index_e+1].curp[j]=0;
            }
            for(auto j:yewu){
                e[index_e].curp[j.second]=1;
                e[index_e+1].curp[j.second]=1;
                Curp[index_e][j.second]=j.first;
                Curp[index_e+1][j.second]=j.first;
            }
            for(int j=0;j<work_path_tmp.size();j++){
                un_do(work_path_tmp[j],RFL_tmp[j]);
            }
            for(auto j:yewu){
                _do(work_path_max[q[j.first].id],j.second,j.first);
            }

            
        }
        break;
    }
    }
    
}

void test1(){
    freopen("4.in","r",stdin);
    freopen("4.out","w",stdout);
    memset(edge_num,0,sizeof(edge_num));
    int seed=11;//11(59356466) 1(59959677)
    st = clock();
    scanf("%d%d%d%d%d",&n,&m,&t,&p,&d);
    init();
    for(int i = 0;i < m;i++){
        int u,v,w;
        scanf("%d%d%d",&u,&v,&w);
        edge_input[i][0]=u;
        edge_input[i][1]=v;
        edge_input[i][2]=w;
        maxedge = max(maxedge,w);
        add(u,v,w,i);
        add(v,u,w,i);
        pair<int, int>tp = make_pair(min(u,v), max(v,u));
        if(mp.count(tp)){
            mp[tp] = min(mp[tp],w);
        }else mp[tp] = w;
    }
    for(int i = 0;i < cnt;i++){
        e[i].curw = maxedge - e[i].w;
    }
    map<pair<int, int> ,int> yewu;
    for(int i = 1;i <= t;i++){
        int u,v;
        scanf("%d%d",&u,&v);
        q_input[i][0]=u;
        q_input[i][1]=v;
        q_input[i][2]=i;
        yewu[make_pair(min(u,v), max(u,v))]++;
        q[i].u = u;
        q[i].v = v;
        q[i].id = i;
    }
    for(int i = 1;i <= t;i++){
        int u,v;
        u = q[i].u;
        v = q[i].v;
        vector<int>nolimitans_edge;
        spfa1(u, v, nolimitans_edge,1);
        q[i].cnt2 = nolimitans_edge.size() + 1;
        q[i].cnt1 = yewu[make_pair(min(u,v), max(u,v))];
    }
    sort(q+1,q+1+t);
    vector<addedge> max_add_edge;
    vector<vector<int>> work_path_max;
    vector<int> RFL_max;
    vector<vector<int>> big_max;
    int max_add=INT_MAX;
    while(true){
        vector<addedge>add_edge;
        vector<vector<int>> work_path(t+1);
        vector<vector<int>> big(t+1);
        vector<int> RFL(t+1);
        re_init();
        curm = m;
        int flag = 1;
        for(int k = 1;k <= t;k++){
            int u = q[k].u;
            int v = q[k].v;
            flag = rand()%2;
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
            if(!(f&&curp!=-1)){
                flag = !flag;
                for(int i = 0;i < p;i++){
                    f = spfa(u,v,pp[i],ans_edge,flag);
                    curp = pp[i];
                    if(f)break;
                }
            }
            //尝试不加边
            //srand(seed++);
            // if(!(f&&curp!=-1)){
            //     bool f1=false;
            //     vector<int> qq_tmp(k);
            //     for(int i=0;i<k;i++){
            //         qq_tmp[i]=i+1;
            //     }
            //     srand(seed++);
            //     random_shuffle(qq_tmp.begin(), qq_tmp.end());
            //     int len_tmp=qq_tmp.size();
            //     for(int qqq=0;qqq<len_tmp;qqq++){
            //         int qq=qq_tmp[qqq];
            //         auto tmp=work_path[q[qq].id];
            //         int rfl_p=RFL[q[qq].id];
            //         //交换两个业务的规划路线，先撤销原有业务的边--，再规划当前业务，再规划原有业务            
            //         un_do(tmp, rfl_p);
            //         vector<int> ans_edgetmp;
            //         int f_ = spfa(u,v,rfl_p,ans_edgetmp,flag);
            //         if(f_){
            //             //试试规划原来的业务，所有波长,在此之前需要将k业务规划占用波长和负载++
            //             _do(ans_edgetmp,rfl_p,k);
            //             vector<int> ans_edgetmp1;
            //             srand(seed++);
            //             random_shuffle(pp.begin(), pp.end());
            //             int uu=q[qq].u;
            //             int vv=q[qq].v;
            //             int cur_p=-1;
            //             for(int i = 0;i < p;i++){
            //                 int f__ = spfa(uu,vv,pp[i],ans_edgetmp1,flag);
            //                 if(f__){
            //                     cur_p=pp[i];
            //                     break;
            //                 }
            //             }
            //             if(cur_p==-1){
            //                 //重新分配qq业务的
            //                 un_do(ans_edgetmp,rfl_p);//加边
            //                 _do(tmp,rfl_p,qq);
            //             }
            //             else{
            //                 //成功，先将qq新规划的路径++
            //                 _do(ans_edgetmp1,cur_p,qq);
            //                 //还需要跟新k和qq业务的全局变量
            //                 vector<int>bigtmp;
            //                 // getbig(ans_edgetmp1, bigtmp);
            //                 big[q[qq].id]=bigtmp;
            //                 RFL[q[qq].id]=cur_p;
            //                 work_path[q[qq].id]=ans_edgetmp1;
            //                 vector<int>bigtmp1;
            //                 // getbig(ans_edgetmp, bigtmp1);
            //                 big[q[k].id]=bigtmp1;
            //                 RFL[q[k].id]=rfl_p;
            //                 work_path[q[k].id]=ans_edgetmp;
            //                 //告诉下面已经分配
            //                 f1=true;
            //                 break;
            //             }
            //         }
            //         else{
            //             //无效，返回,路径和波长都不变
            //             _do(tmp,rfl_p,qq);
            //         }
            //     }
            //     if(f1){
            //         continue;
            //     }
            // }
            
            if(f&&curp!=-1){
                vector<int>bigtmp;
                // getbig(ans_edge, bigtmp);
                big[q[k].id]=bigtmp;
                RFL[q[k].id]=curp;
                _do(ans_edge,curp,k);
                work_path[q[k].id]=ans_edge;
            }else{
                vector<int>nolimitans_edge;
                spfa1(u, v, nolimitans_edge,flag);
                int addedgemin = -1;
                vector<addedge>nowadd;
                int maxloadnumid = 0;
                for(int i = 0;i < p;i++){
                    f = spfa(u,v,i,work_path[q[k].id],flag);
                    vector<addedge>curadd;
                    int loadnum = 0;
                    int curmaxloadnumid = 0;
                    int curmaxloadnum = -1;
                    for(int ie = 0;ie < nolimitans_edge.size();ie++){
                        int uu = e[nolimitans_edge[ie]].u;
                        int vv = e[nolimitans_edge[ie]].v;
                        if(dis[uu]!=inf&&dis1[vv]!=inf&&dis[vv]==inf){
                            if(Curp[nolimitans_edge[ie]][i]==0){
                                //cout<<"error"<<endl;
                            }
                            curadd.push_back(addedge{uu,vv});
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
                e[cnt-1].curw = maxedge - e[cnt-1].w;
                e[cnt-2].curw = maxedge - e[cnt-2].w;
                curm++;
                add_edge.push_back(addedge{uu,vv});
                k--;
            }
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
        if(double(et-st)/CLOCKS_PER_SEC>30){
            break;
        }
        
    }
    curm=(int)max_add_edge.size()+m;
   // cout<<curm<<endl;
    //localserch
    // int count_edge_num=0;
    // int count_edge_num1=0;
    // for(int i=2*m;i<2*curm;i=i+2){
    //     if(edge_num[i]==0&&edge_num[i+1]==0){
    //         count_edge_num++;
    //     }
    //     bool f_num=false;
    //     for(int j=0;j<p;j++){
    //         if(Curp[i][j]!=0&&Curp[i+1][j]!=0){
    //             f_num=true;
    //         }
    //     }
    //     if(f_num){
    //         count_edge_num1++;
    //     }
    // }
    // cout<<count_edge_num<<"--"<<count_edge_num1<<endl;
   // cout<<Curp[3755*2][2]<<" "<<Curp[3755*2+1][2]<<endl;
    int count_edge_num=0;
    int count_edge_num1=0;
   localsearch1(work_path_max,RFL_max,seed);
    //cout<<Curp[3755*2][2]<<" "<<Curp[3755*2+1][2]<<endl;
    // for(int i=2*m;i<2*curm;i+=2){
    //    // cout<<edge_num[i]<<"----"<<edge_num[i+1]<<endl;
    //     if(edge_num[i]==0&&edge_num[i+1]==0){
    //         count_edge_num++;
    //     }
    //     bool f_num=false;
    //     for(int j=0;j<p;j++){
    //         if(Curp[i][j]!=0&&Curp[i+1][j]!=0){
    //             f_num=true;
    //         }
    //     }
    //     if(f_num){
    //         count_edge_num1++;
    //     }
    // }
    //cout<<count_edge_num<<"--"<<count_edge_num1<<endl;
     vector<int> sub(curm,-1);
    vector<int> add_tmp;
    int sum=0;
     vector<addedge> add_edge_tmp;
    for(int i=2*m;i<2*curm;i+=2){
        bool f_num=false;
        for(int j=0;j<p;j++){
            if(Curp[i][j]!=0&&Curp[i+1][j]!=0){
                f_num=true;
            }
        }
        if(f_num==false){
            //此边删去
            sum++;
        }
        else{
            add_edge_tmp.emplace_back(max_add_edge[i/2-m]);
            sub[i/2]=i/2-sum;
        }
    }
    max_add_edge=add_edge_tmp;

   // cout<<ct<<endl;
    //et = clock();
   // cout<<double(et-st)/CLOCKS_PER_SEC<<endl;
    vector<vector<int>> final(t+1);
    for(int k=1;k<=t;k++){
        vector<int> fin;
        fin.push_back(RFL_max[q[k].id]);
        fin.push_back((int)work_path_max[q[k].id].size());
        getbigcopy(work_path_max[q[k].id],big_max[q[k].id]);
        fin.push_back((int)big_max[q[k].id].size());
        for(int i=0;i<work_path_max[q[k].id].size();i++){
            if(sub[e[work_path_max[q[k].id][i]].id]!=-1){
                fin.push_back(sub[e[work_path_max[q[k].id][i]].id]);
            }
            else{
                fin.push_back(e[work_path_max[q[k].id][i]].id);
            }
            
        }
        for(int i = 0;i < big_max[q[k].id].size();i++){
            fin.push_back(big_max[q[k].id][i]);
        }
        final[q[k].id] = fin;
    }
    printf("%d\n",(int)max_add_edge.size());
    for(int i = 0;i < max_add_edge.size();i++){
        printf("%d %d\n",max_add_edge[i].u,max_add_edge[i].v);
    }

    for(int i = 1;i <= t;i++){
        for(int j = 0;j < final[i].size();j++){
            printf("%d ",final[i][j]);
        }
        printf("\n");
    }
}





int main(){
    test1();
   // test2();
  // test3();
}
/*
7 10 6 4 6 
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
0 6
2 4 
2 4  
2 4  
2 4 
2 4
*/