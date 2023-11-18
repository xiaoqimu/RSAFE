#include<bits/stdc++.h>
#include <bitset>
#include <cassert>
#include <climits>
#include <queue>
#include <utility>
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
    bool f_lianxu; // 该业务是否需要连续波长,true连续；false 不连续
    vector<vector<int> > k_short_path;//存储当前业务的k条最短路
    vector<int> p_id;
    vector<int> ans_edge_id;
    int targetpathindex = -1;//在k_short_path中的位置
    query_input(){}
    query_input(int u,int v,int id,int num,bool f_lianxu):u(u),v(v),id(id),num(num),f_lianxu(f_lianxu){}
    bool operator<(const query_input &A) const {
        return num > A.num;
    }
    
};
struct work_answer{
    int m_cnt,n_cnt;
    vector<int> p_id;
    vector<int> ans_edge_id;

    vector<vector<int> >k_short_path;
};

struct addedge{//记录加边的两个端点
    int u,v;
    addedge(){}
    addedge(int u,int v):u(u),v(v){}
};

vector<bitset<maxn> >conflict;//计算每条业务经过边的冲突度

/*
存储每条边在输入中的位置
*/
map<pair<int, int>, vector<int> > edge_pos; 

/*
存图相关的数据结构
*/
struct edge{
    int u,v,w,next,curw;
    int id;//id表示当前边在输入中的顺序
    int pos;//pos表示当前边在链式前向星中的下标
    int curp[maxp];//记录当前边波长的占用情况
    int max_p_num;//当前的最大的波长
    int p_nums;//记录当前经过的业务数。
    int used;//记录是否使用
    int c_e;
    int curpos_inedgepos;//记录当前边目前使用的位置
    bitset<maxp> pset;//当前边波长占用情况，求集合
};
int head[maxn];
edge e[maxm<<1];//记录图中边的集合
int cnt;//记录边的个数，边是成对出现0和1、2和3

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
功能：新增边，一条u->v的有向边
参数：u、v表示这条边的两个端点，w代表这条边的权值，id对应这条边在输入中的顺序
*/
inline void add(int u,int v,int w,int id){
	e[cnt].next = head[u];
    e[cnt].u = u;e[cnt].v = v;
	e[cnt].w = w;e[cnt].id = id;
    for(int i=0;i<P;i++){
        e[cnt].pset[i]=1;
    }
    e[cnt].pos = cnt;head[u] = cnt++; 
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
            if(e[i].used==1)continue;
            int w= e[i].curw;
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
    vector<int>res_dege;
    int sta = spfa1_normal(t,s,res_dege);
    if(sta==0)return 0;
    for(int i = 0;i < N;i++){
        h[i] = dis1[i];
    }

    //到这一步表示s到t有路径可达，不管有没有限制
    priority_queue<astarnode>q;
    vector<int>path;
    q.push(astarnode(s,h[s],0,path));
    int cnt = 0;
    int cursize = 0;
    int node_num=0;
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
            node_num=0;
            continue;
            //cout <<"边大小：" <<ans_edge.size() << endl;
        }
        ++node_num;
        if(++node_num>=10*N)break;
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
    if(ans_edge.size()==0){
        spfa1_normal(s,t,res_dege);
        ans_edge.push_back(res_dege);
    }
    
    return 1;
}

bool checkfile(const char *filename){
    FILE *fp=fopen(filename,"r");
    if(fp!=NULL){
        fclose(fp);
        return 1;
    }
    return 0;
}

void changew(const vector<int> &edges){
    for(int i = 0;i < edges.size();i++){
        e[edges[i]].curw++;
        if(edges[i]&1){
            e[edges[i]-1].curw++;
        }else{
            e[edges[i]+1].curw++;
        }
    }
}

/*
功能：处理所有的输入，并计算最大的边权值和记录每条边的最小权值
参数：edge 表示输入的边集合，query表示输入的业务集合
*/
void readfile(const string &pathin,const string &pathout,vector<edge_input> &edge,vector<query_input>&query){
    freopen(pathin.c_str(),"r",stdin);
    
    scanf("%d%d%d%d",&N,&M,&T,&P);
    for(int i = 0;i < M;++i){
        int u,v,w;
        scanf("%d%d",&u,&v);
        w = 1;
        edge.push_back(edge_input(u,v,w));
    }
    for(int i = 0;i < T;++i){
        int u,v,num;
        scanf("%d%d%d",&u,&v,&num);
        query.push_back(query_input(u,v,i,num,true));
    }
}


/*
功能：把输入的边构建成一个图
参数：要构建图的边集合
*/
void constructgraph(const vector<edge_input> &edge){
    //初始化图的数据结构
    init();
    assert(edge.size() == M);
    map<pair<int, int>, int> edge_vis;
    //开始构建图
    for(size_t i = 0;i < edge.size();++i){
        int u,v,w;
        u = edge[i].u;
        v = edge[i].v;
        w = edge[i].w;

        pair<int,int>u_v = make_pair(min(u, v), max(u, v));
        if (!edge_vis.count(u_v)) {
            edge_vis[u_v] = 1;
            add(u,v,w,i);
            add(v, u, w, i);
        }
        vector<int>edge_p;
        if (edge_pos.count(u_v)) {
            edge_p = edge_pos[u_v];
        }
        edge_p.push_back(i);
        edge_pos[u_v] = edge_p;
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
    // map<pair<int, int> ,int> work;//业务重复次数
    // assert(query.size()==T);
    // for(size_t i = 0;i < query.size();++i){
    //     int u,v;
    //     u = query[i].u;
    //     v = query[i].v;
    //     work[make_pair(min(u,v), max(u,v))]++;
    // }
    // conflict.clear();
    // for(size_t i = 0;i < query.size();++i){
    //     int u,v;
    //     u = query[i].u;
    //     v = query[i].v;
    //     vector<int>ans_edge;
    //     spfa1_normal(u,v,ans_edge);//不带限制的最短路权重为1.
    //     query[i].cnt1 = work[make_pair(min(u,v), max(u,v))];
    //     query[i].cnt2 = ans_edge.size() + 1;

    //     cal_conflict(ans_edge);
    // }

    sort(query.begin(),query.end());
}




void printanswer(string pathout,const vector<addedge> & add_edge,const vector<work_answer>& ans,const vector<query_input>&query){
    //输出新增的边
    freopen(pathout.c_str(),"w",stdout);
    printf("%d\n",(int)add_edge.size());
    for(size_t i = 0;i < add_edge.size();++i){
        printf("%d %d\n",add_edge[i].u,add_edge[i].v);
    }
    //输出每条业务的答案
    for(size_t i = 0;i < ans.size();++i){
        // printf("第 %d 条业务的情况：\n",(int)i);
        // printf("经过的编号: ");
        for(size_t j = 0;j < ans[i].p_id.size();++j){
            printf("%d ",ans[i].p_id[j]);
        }
        // printf("\n");
        // printf("%d %d ",ans[i].m_cnt,ans[i].n_cnt);
        // printf("经过的边的编号：");
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


int short_path(int s,int t,vector<vector<int> >&ans_edge,vector<edge_input>edge_input){
    ans_edge.clear();
    // constructgraph(edge_input);
    vector<int>nowans;
    int f = spfa1_normal(s,t,nowans);
    ans_edge.push_back(nowans);
    return 1;
}

bool find_path_for_allwork(vector<query_input> &query,vector<edge_input>edge_input,int k) {
    bool f = 1;
    for (int i = 0; i < query.size(); i++) {
        cout << "第" << i << "条业务" << endl;
        f &=  k_short_path(query[i].u,query[i].v,k,query[i].k_short_path);
        assert(query[i].k_short_path.size()>=1);
       // changew(query[i].k_short_path[0]);
        //cout << query[i].k_short_path.size() << "\n";
    }
    return f;
}

int cal_min_pnum(const vector<int> &path){
    int max_pnum = 0;
    for (int j = 0; j < path.size();j++){
        max_pnum = max(e[path[j]].p_nums, max_pnum);
    }
    return max_pnum;
}
struct query_parm{
    pair<int,int>query;
    int fd,ld,lp;
};
int query_vis[maxn];
void cal_targetpath(const vector<query_input> &query,vector<query_input>&allquery) {

    int querycnt = query.size();
    for(int i = 0;i < query.size();i++){
        for(int j = 0;j < query[i].k_short_path.size();j++){
            for(int k = 0;k < query[i].k_short_path[j].size();k++){
                int pos = query[i].k_short_path[j][k] ;
                e[pos].c_e += query[i].num;
                if(pos&1)e[pos-1].c_e += query[i].num;
                else e[pos+1].c_e += query[i].num;
            }
        }
    }
    while(querycnt>0){
        vector<query_parm>nowquery;
        
        //统计目前哪些是还存在的
        for(int i = 0;i < query.size();i++){
            if(query_vis[query[i].id]==1)continue;
            int ld = 0;
            for(int j = 0;j < query[i].k_short_path.size();j++){
                int fd = cal_min_pnum(query[i].k_short_path[j]);
                int lp = 0;
                for(int k = 0;k < query[i].k_short_path[j].size();k++){
                    lp += e[query[i].k_short_path[j][k]].c_e;
                }
                ld += lp;
                nowquery.push_back({make_pair(query[i].id,j),fd,-1,lp});
                
            }
            ld /= (int)query[i].k_short_path.size();
            for(int j = 0;j < query[i].k_short_path.size();j++){
                nowquery[nowquery.size()-1-j].ld = ld;
            }
        }
        for(int i = 0;i < nowquery.size();i++){
            assert(nowquery[i].ld!=-1);
        }
        sort(nowquery.begin(),nowquery.end(),[](query_parm A,query_parm B){
            if(A.fd==B.fd){
                if(A.ld==B.ld){
                    return A.lp < B.lp;
                }
                return A.ld < B.ld;
            }
            return A.fd < B.fd;
        });
        int queryid = nowquery[0].query.first;
        int querypathindex = nowquery[0].query.second;
        allquery[queryid].targetpathindex = querypathindex;
        query_vis[queryid] = 1;
        for (int i = 0; i < allquery[queryid].k_short_path[querypathindex].size();i++){
            int pos = allquery[queryid].k_short_path[querypathindex][i];
            e[pos].p_nums += allquery[queryid].num;
            if(pos&1){
                e[pos-1].p_nums += allquery[queryid].num;
            }else e[pos+1].p_nums += allquery[queryid].num;
        }
        querycnt--;
        
    }
}

void allocation(query_input &query,vector<addedge>&add_edge){
    vector<int> path = query.k_short_path[query.targetpathindex];
    static int curm = M;
    int min_remain = INT_MAX;
    for (int j = 0; j < path.size();j++){
        int nowremain = P - e[path[j]].max_p_num;
        //cout << nowremain << "\n";
        if(nowremain<query.num){//需要新增边
            
            e[path[j]].curpos_inedgepos++;
            e[path[j]].max_p_num = 0;

            if(path[j]&1){
                e[path[j]-1].curpos_inedgepos++;
                e[path[j]-1].max_p_num = 0;
            }else{
                e[path[j]+1].curpos_inedgepos++;
                e[path[j]+1].max_p_num = 0;
            }

            int u, v;
            u = e[path[j]].u;
            v = e[path[j]].v;
            vector<int> edge_p = edge_pos[make_pair(min(u, v), max(u, v))];
            if(e[path[j]].curpos_inedgepos==edge_p.size()){//判断是否还有其他边
                add(u,v,1,curm);
                add(v,u,1,curm);
                
                add_edge.push_back(addedge{u,v});
                // cout << "curm:" << curm << "\n";
                edge_p.push_back(curm);
                edge_pos[make_pair(min(u, v), max(u, v))] = edge_p;  
                curm++;
            }
            
        }
    }
    int MaxPnum = 0;
    for (int j = 0; j < path.size();j++){
        MaxPnum = max(MaxPnum, e[path[j]].max_p_num);
    }
    vector<int> p_id, ans_edge;
    for (int i = 0; i < query.num;i++){
        p_id.push_back(MaxPnum + i);
    }
    query.p_id = p_id;
    // cout <<"MaxPnum:" <<MaxPnum << endl;
    for (int j = 0; j < path.size(); j++){
        
        e[path[j]].max_p_num = MaxPnum + query.num;

        if(path[j]&1){
            e[path[j]-1].max_p_num = MaxPnum + query.num;
        }else{
            e[path[j]+1].max_p_num = MaxPnum + query.num;
        }
        // cout << e[path[j]].max_p_num << "\n";
        int u, v;
        u = e[path[j]].u;
        v = e[path[j]].v;
        vector<int> edge_p = edge_pos[make_pair(min(u, v), max(u, v))];
        ans_edge.push_back(edge_p[e[path[j]].curpos_inedgepos]);
        assert(e[path[j]].max_p_num <= P);
    }
    query.ans_edge_id = ans_edge;
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

    int task[10000][3]; //业务起点 终点 和需要波长数量
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

    vector<int> result_bochang[10010];
    vector<int> result_luyou[10010];
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
    
    printf("check end!\n");
    return 1;
}

int main(){
    vector<string> name{"enron-only.in","infect-dublin.in","crime-moreno.in","email-univ.in"};
    auto start = chrono::steady_clock::now();
    auto ed = start;
    vector<edge_input>edge_input;//存储输入的边 u-v表示边、w表示边权
    vector<query_input>q_input;//存储输入的查询 u-v表示边、id表示输入的顺序   
    //读取输入
    string pathin = "E:\\c++\\paper\\ConsoleApplication1\\email-univ6.in";
    string pathout = "E:\\c++\\paper\\ConsoleApplication1\\FA_email-univ6.out";

    readfile(pathin,pathout,edge_input, q_input);

    //构建图
    constructgraph(edge_input);

    //业务排序
    calworkorders(q_input);
    // for(int i = 0;i < T;++i){
    //     printf("%d %d
    //     %d\n",q_input[i].u,q_input[i].v,q_input[i].id);
    // }
    int k = 1;
    find_path_for_allwork(q_input, edge_input,k);
    //为每一个业务在其选择的k条最短路中找一条最适合的
    vector<vector<query_input> >Dn(maxp+1);
    for (int i = 0; i < q_input.size();i++){
        Dn[q_input[i].num].push_back(q_input[i]);
    }
    for(int i = P;i > 0;i--){
        if(Dn[i].size()==0)continue;
        cal_targetpath(Dn[i],q_input);
    }
    vector<addedge> addedge;
    for (int i = 0; i < q_input.size();i++){
        assert(q_input[i].targetpathindex!=-1);
        //cout << "第 " << i << "条业务:"<<endl;
        allocation(q_input[i], addedge);
    }

    // 处理结果
    vector<work_answer> ans(T);
    for(int i = 0;i < T;++i){
        work_answer work_ans;
        work_ans.ans_edge_id = q_input[i].ans_edge_id;
        work_ans.p_id = q_input[i].p_id;
        ans[q_input[i].id] = work_ans;
    }

    //输出答案
    printanswer(pathout,addedge, ans, q_input);
    cout << "add edge:" << (int)addedge.size() << "\n";
    ed = chrono::steady_clock::now();
    chrono::duration<double> dif = ed - start;
    cout << "spend time:" << dif.count() << "\n";
    check(pathin.c_str(),pathout.c_str());
    return 0;
}

