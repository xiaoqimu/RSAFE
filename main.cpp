#include<bits/stdc++.h>
#include <climits>
#include <cstdint>
#include <utility>
#include <vector>
using namespace std;
typedef long long ll;
const int maxm = 2e5+10;
const int maxn = 5010;
int n,m,t,p,d,curm;
int seed;//11(59356466) 1(59959677)
int head[maxn];
int headcopy[maxn];
struct edge{
    int u,v,w,id,curp[85],next,pos;
    int loadnum,curw;
    int cost = 10;
};
struct node{
    int u,id;
};
struct addedge{
    int u,v;
};
int Curp[maxm<<1][85];
const int inf = 0x3f3f3f3f;
node pre[maxn];
edge e[maxm<<1],ecopy[maxm<<1];
int vis[maxn];
int dis[maxn];
int maxedge = 0;
int cnt;
void init(){
	memset(head,-1,sizeof(head));
	cnt = 0;
}
void add(int u,int v,int w,int id){
	e[cnt].next = head[u];
    e[cnt].u = u;e[cnt].v = v;
	e[cnt].w = w;e[cnt].id = id;
    e[cnt].pos = cnt;head[u] = cnt++; 
}

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

void _do(const vector<int>& path,int aimp,int q_){
    //撤销边的负载和波长
    for(auto i:path){
        e[i].curp[aimp]=1;//未使用
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

    while(pre[curv].u!=-1){
      
        ans_edge.push_back(pre[curv].id);
   
        curv = pre[curv].u;
  
    }
    reverse(ans_edge.begin(),ans_edge.end());
    
    return 1;
}
int dis1[maxn];
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
            // if(f==1){
            //     w = e[i].w;
            // }else w = e[i].curw;
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

struct query{
    int u,v,id;
    int cnt1,cnt2;
    int cnt3,cnt4;
};


inline bool cmd_query_cnt1(const query& a,const query& b){
    return a.cnt1 > b.cnt1;
}

inline bool cmd_query_cnt2(const query& a,const query& b){
    return a.cnt2 > b.cnt2;
}
inline bool cmd_query_cnt2_cnt1(const query& a,const query& b){
    if(a.cnt2==b.cnt2)return a.cnt1 > b.cnt1;
    return a.cnt2 > b.cnt2;
}

inline bool cmd_query_cnt2_cnt4_cnt1(const query& a,const query& b){
    if(a.cnt2==b.cnt2){
        if(a.cnt3==b.cnt3)return a.cnt1 > b.cnt1;
        return a.cnt3 > b.cnt3;
    }
    else{
        return a.cnt2>b.cnt2;
    }

}
inline bool cmd_query_cnt4_cnt2_cnt1(const query& a,const query& b){
    if(a.cnt3==b.cnt3){
        if(a.cnt2==b.cnt2)return a.cnt1 > b.cnt1;
        return a.cnt2 > b.cnt2;
    }
    else{
        return a.cnt3>b.cnt3;
    }

}

query q[10010];
//vector<vector<int>> final;
int edge_input[maxm][3];
int q_input[10000][3];
int edge_num[1000000];
map< pair<int, int>, int>mp;
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

void updateedge(int n,int cnt){
    for(int i = 0;i < n;i++){
        headcopy[i] = head[i];
    }
    for(int i = 0;i < cnt;i++){
        ecopy[i] = e[i];
    }
}

vector<bitset<5000>> conflict;
void cal_conflict(const vector<int> &nolimitans_edge){
    bitset<5000>edge_conflict;
    for(int i = 0;i < nolimitans_edge.size();i++){
        edge_conflict[e[nolimitans_edge[i]].id] = 1;
    }
    conflict.push_back(edge_conflict);
}


bool spfa_(int u,int v,vector<vector<int>>& ans_edgetmp1,vector<vector<int>>& rfl_tmp1,int pathcnt){
    //求出业务的路径的路由和对应的波长分配。
    //返回true:成功；false:失败
    

}
// spfa_(u,v,rfl_p,ans_edgetmp,r);求业务k的r条边，使用波长返回.
bool rollback_2(int k,vector<int>& yewu,vector<vector<vector<int>>>& work_path,vector<vector<int>>& RFL,vector<int>& pp,int flag){
    bool f1=false;
    int u=query[k].u;
    int v=query[k].v;
    int pathcnt=query[k].pathcnt;
    //撤销
    for(auto& qq:yewu){
        for(int i=0;i<query[qq].pathcnt;i++){
            auto tmp=work_path[query[qq].id][i];
            auto rfl_p=RFL[query[qq].id][i];
            un_do(tmp,rfl_p);
        } 
    }
    //先排当前业务，用撤回的波长
    vector<vector<int>> ans_edgetmp;
    vector<vector<int>> rfl_tmp;
    //求当前业务的路由，带限制版本;返回路径和波长
    int f1_ = spfa_(u,v,ans_edgetmp,rfl_tmp,query[k].pathcnt);
    if(f1_){
        //当前波长可以有新的路径，试着将其分配，并将撤回的业务重新分配。
        for(int i=0;i<ans_edgetmp.size();i++){
            _do(ans_edgetmp[i],rfl_tmp[i],k);
        }
        bool f__=false;
        vector<vector<vector<int>>> work_path_tmp;//记录新的路径
        vector<vector<vector<int>>> RFL_tmp;
        for(auto& qq_tmp:yewu){
            //重新分配
            vector<vector<int>> ans_edgetmp1;
            vector<vector<int>> rfl_tmp1;
            int uu=query[qq_tmp].u;
            int vv=query[qq_tmp].v;
            int f_=spfa_(uu,vv,ans_edgetmp1,rfl_tmp1,query[qq_tmp].pathcnt);
            if(f_==0){
                //该业务重新分配失败
                f__=true;
                break;
            }
            else{
                //当前业务成功，还要继续分配
                work_path_tmp.emplace_back(ans_edgetmp1);
                RFL_tmp.emplace_back(rfl_tmp1);
                for(int i=0;i<query[qq_tmp].pathcnt;i++){
                    _do(ans_edgetmp1[i],rfl_tmp1[i],qq_tmp);
                }
            }
        }
        if(f__){
            //相同波长业务分配失败，需要回滚到最初状态
            for(int i=0;i<ans_edgetmp.size();i++){
                un_do(ans_edgetmp[i],rfl_tmp[i]);
            }
            for(int i=0;i<work_path_tmp.size();i++){
                //对相关波长业务，进行回滚
                for(int j=0;j<work_path_tmp[i].size();j++){
                    un_do(work_path_tmp[i][j],RFL_tmp[i][j]);
                }
                
            }
            for(auto& qq_tmp:yewu){
                for(int i=0;i<query[qq_tmp].pathcnt;i++){
                    _do(work_path[query[qq_tmp].id][i],RFL[query[qq_tmp].id][i],qq_tmp);
                }
                
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
        for(auto& qq_tmp:yewu){
            for(int i=0;i<query[qq_tmp].pathcnt;i++){
                _do(work_path[query[qq_tmp].id][i],RFL[query[qq_tmp].id][i],qq_tmp);
            }
        }
    }

    return f1;

}




bool rollback(int k,vector<int>& yewu,vector<vector<int>>& work_path,vector<int>& RFL,vector<int>& pp,int flag){
    //只有相同波长的业务一起撤销才有效,不同波长互不干扰
    bool f1=false;
    int u=q[k].u;
    int v=q[k].v;
    unordered_map<int,vector<int>> rfl_p_pre;//不同波长对于的业务
    for(auto& qq:yewu){
        int rfl_p=RFL[q[qq].id];
        rfl_p_pre[rfl_p].emplace_back(qq);
    }
    for(auto& ii:rfl_p_pre){
        int rfl_p=ii.first;//该波长去重新规划业务k的路
        for(auto& qq_tmp:ii.second){
            auto tmp=work_path[q[qq_tmp].id];
            un_do(tmp,rfl_p);
        }
        //先排当前业务，用撤回的波长
        vector<int> ans_edgetmp;
        int f1_ = spfa(u,v,rfl_p,ans_edgetmp,flag);
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
                int uu=q[qq_tmp].u;
                int vv=q[qq_tmp].v;
                int cur_p=-1;
                for(int i=0;i<pp.size();i++){
                    int f_=spfa(uu,vv,pp[i],ans_edgetmp1,flag);
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
                    auto tmp=work_path[q[qq_tmp].id];
                    _do(tmp,rfl_p,qq_tmp);
                }
                //next bochang
            }
            else{
                //this bochang重新分配成功
                f1=true;
                for(int i=0;i<work_path_tmp.size();i++){
                    RFL[q[ii.second[i]].id]=RFL_tmp[i];
                    work_path[q[ii.second[i]].id]=work_path_tmp[i];
                }
                RFL[q[k].id]=rfl_p;
                work_path[q[k].id]=ans_edgetmp;
                return f1;
            }
        }
        else{
            for(auto& qq_tmp:ii.second){
                auto tmp=work_path[q[qq_tmp].id];
                _do(tmp,rfl_p,qq_tmp);
            }
        }
    
    }

    return f1;
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

void localsearch(vector<vector<int>>& work_path_max,vector<int>& RFL_max,int& seed,vector<int>& sub,vector<addedge>& max_add_edge){

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
    sub.clear();
    sub.resize(curm,-1);
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
}

void test1(){
    freopen("4.in","r",stdin);
    freopen("4.out","w",stdout);
    st = clock();
    seed=11;
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
    conflict.clear();
    vector<vector<int>> Curpp(m);//记录每条边的业务
    for(int i = 1;i <= t;i++){
        int u,v;
        u = q[i].u;
        v = q[i].v;
        vector<int>nolimitans_edge;
        spfa1(u, v, nolimitans_edge,1);
        q[i].cnt2 = nolimitans_edge.size() + 1;
        q[i].cnt1 = yewu[make_pair(min(u,v), max(u,v))];
        //cal_conflict(nolimitans_edge);
        for(auto &ie:nolimitans_edge){
            Curpp[e[ie].id].push_back(i);
        }
    }
    //cnt3,cnt4
    // for(int i = 0;i < conflict.size();i++){
    //     int cnt = 0;
    //     for(int j = 0;j < conflict.size();j++){
    //         if(i==j)continue;
    //         if((conflict[i]&conflict[j])!=0)cnt++;
    //     }
    //     q[i+1].cnt3 = cnt;
    // }
    for(auto &i:Curpp){
        int tmp_l=i.size();
        for(auto &j:i){
            q[j].cnt4+=tmp_l;
        }
    }
    sort(q+1,q+1+t,cmd_query_cnt4_cnt2_cnt1);
    vector<addedge> max_add_edge;
    vector<vector<int>> work_path_max;
    vector<int> RFL_max;
    vector<vector<int>> big_max;
    int max_add=INT_MAX;
    int tmp_p=p;
    while(true){
        //p=ceil(tmp_p/1.5);
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
            if(!(f&&curp!=-1)){
                bool f1=false;
                vector<int> qq_tmp(k-1);
                for(int i=0;i<k-1;i++){
                    qq_tmp[i]=i+1;
                }
                srand(seed++);
                random_shuffle(qq_tmp.begin(), qq_tmp.end());
                int len_tmp=qq_tmp.size();
                for(int qqq=0;qqq<len_tmp;qqq++){
                    vector<int> yewu_tmp{qq_tmp[qqq]};
                    f1=rollback(k,yewu_tmp,work_path,RFL,pp,flag);
                    if(f1){
                        break;
                    }
                }
                if(f1){
                    continue;
                }
                for(int qqq=0;qqq<len_tmp/10;qqq++){
                    for(int qqqq=qqq+1;qqqq<len_tmp/10;qqqq++){
                        if(RFL[q[qq_tmp[qqq]].id]!=RFL[q[qq_tmp[qqqq]].id]){
                            continue;
                        }
                        vector<int> yewu_tmp{qq_tmp[qqq],qq_tmp[qqqq]};
                        f1=rollback(k,yewu_tmp,work_path,RFL,pp,flag);
                        if(f1){
                            break;
                       }
                    }
                    if(f1){
                        break;
                    }
                }
                if(f1){
                    continue;
                }
            }
            if(f&&curp!=-1){
                vector<int>bigtmp;
                // getbig(ans_edge, bigtmp);
                big[q[k].id]=bigtmp;
                RFL[q[k].id]=curp;
                _do(ans_edge,curp,k);
                work_path[q[k].id]=ans_edge;
            }else{
                // if(p<tmp_p){
                //     p++;
                //     k--;
                //     continue;
                // }
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
                //cout<<maxloadnumid<<endl;
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
        if(double(et-st)/CLOCKS_PER_SEC>10){
            break;
        }
        
    }
    curm=(int)max_add_edge.size()+m;
    vector<int> sub(curm,-1);
    //localsearch(work_path_max,RFL_max,seed,sub,max_add_edge);
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
    et=clock();
    cout<<double(et-st)/CLOCKS_PER_SEC<<endl;
}