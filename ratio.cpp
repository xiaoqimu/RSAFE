#include<bits/stdc++.h>
#include <fstream>
using namespace std;

int small_ratio(){
    //结果文件输入
    //输出ratio.csv作图数据
    string path="E:\\c++\\paper\\ConsoleApplication1\\";
    vector<string> s_vec{"","JRSAFE_","WOH_","BLSA_","MBLSA_","FA_"};
    vector<string> s1_vec{"NSFNET","USNET","Japan","Spanish"};
    vector<vector<double>> ans;
    vector<string> ans_name;
    for(int k=0;k<4;k++){
        for(int j=1;j<=6;j++){
            vector<double> tmp;
            string pathin = s1_vec[k]+to_string(j)+".in";
            for(int i=0;i<6;i++){
                string pathout = s_vec[i]+  s1_vec[k]+to_string(j)+".out";
                if(i==0){
                    auto l=freopen((path+pathout).c_str(),"r",stdin);
                    if(l==0){
                        //没有解
                        break;
                    }
                }
                //记录改解的占用率
                auto l3=freopen((path+pathout).c_str(),"r",stdin);
                assert(l3!=0);
                int addEdgeNum;
                scanf("%d",&addEdgeNum);
                auto l2=freopen((path+pathin).c_str(),"r",stdin);
                assert(l2!=0);
                int N,M,T,P; 
                scanf("%d%d%d%d",&N,&M,&T,&P);
                const int max_edge=M+addEdgeNum;
                vector<vector<int>> edge(max_edge,vector<int>(2));
                for(int i = 0;i < M;i++){
                    int w=1;
                    scanf("%d%d",&edge[i][0],&edge[i][1]);
                }
                int task[10000][3]; //业务起点 终点 和需要波长数量
                for(int i = 0;i < T;i++){
                    scanf("%d%d%d",&task[i][0],&task[i][1],&task[i][2]);
                }
                auto l4=freopen((path+pathout).c_str(),"r",stdin);
                assert(l4!=0);
                int addNum;
                scanf("%d",&addNum);
                for(int i = M;i < M + addNum;i++){
                    scanf("%d%d",&edge[i][0],&edge[i][1]);
                    if(edge[i][0] >=N || edge[i][1] >= N) {
                        cout<<"error"<<endl;
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
                double yes_count=0;
                for(auto& i:channel){
                    for(auto& j:i){
                        if(j==1){
                            yes_count++;
                        }
                    }
                }
                double ans_r=(double)(yes_count)/(double)((M+addEdgeNum)*P);
                
                tmp.push_back(ans_r);
            }
            if(!tmp.empty()){
                ans.push_back(tmp);
                ans_name.push_back(pathin);
            }

        }
        
    }
    //计算gap

    std::ofstream outfile(path+"small_ratio.csv");  // 创建并打开CSV文件
    //有解的实例ratio
    if (!outfile) {
        std::cerr << "无法打开文件！" << std::endl;
        return 1;
    }
    //一行表示一个实例的各种算法的ratio，前提是ILP存在解。
    for(int i=0;i<ans.size();i++){
        outfile<<ans_name[i]<<",";
        for(int j=0;j<ans[i].size();j++){
            outfile<<ans[i][j]<<",";
        }
        outfile<<endl;
    }

    outfile.close();  // 关闭文件
}

int large_ratio(){
    //结果文件输入
    //输出ratio.csv作图数据
    string path="E:\\c++\\paper\\ConsoleApplication1\\";
    vector<string> s_vec{"JRSAFE_","WOH_","BLSA_","MBLSA_","FA_"};
    vector<string> s1_vec{"enron-only","infect-dublin","crime-moreno","email-univ"};
    vector<vector<double>> ans;
    vector<string> ans_name;
    for(int k=0;k<4;k++){
        for(int j=1;j<=6;j++){
            vector<double> tmp;
            string pathin = s1_vec[k]+to_string(j)+".in";
            for(int i=0;i<5;i++){
                string pathout = s_vec[i]+ s1_vec[k]+to_string(j)+".out";
                if(i==0){
                    auto l=freopen((path+pathout).c_str(),"r",stdin);
                    if(l==0){
                        //没有解
                        break;
                    }
                }
                //记录改解的占用率
                auto l3=freopen((path+pathout).c_str(),"r",stdin);
                assert(l3!=0);
                int addEdgeNum;
                scanf("%d",&addEdgeNum);
                auto l2=freopen((path+pathin).c_str(),"r",stdin);
                assert(l2!=0);
                int N,M,T,P; 
                scanf("%d%d%d%d",&N,&M,&T,&P);
                const int max_edge=M+addEdgeNum;
                vector<vector<int>> edge(max_edge,vector<int>(2));
                for(int i = 0;i < M;i++){
                    int w=1;
                    scanf("%d%d",&edge[i][0],&edge[i][1]);
                }
                int task[10000][3]; //业务起点 终点 和需要波长数量
                for(int i = 0;i < T;i++){
                    scanf("%d%d%d",&task[i][0],&task[i][1],&task[i][2]);
                }
                auto l4=freopen((path+pathout).c_str(),"r",stdin);
                assert(l4!=0);
                int addNum;
                scanf("%d",&addNum);
                for(int i = M;i < M + addNum;i++){
                    scanf("%d%d",&edge[i][0],&edge[i][1]);
                    if(edge[i][0] >=N || edge[i][1] >= N) {
                        cout<<"error"<<endl;
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
                double yes_count=0;
                for(auto& i:channel){
                    for(auto& j:i){
                        if(j==1){
                            yes_count++;
                        }
                    }
                }
                double ans_r=(double)(yes_count)/(double)((M+addEdgeNum)*P);
                
                tmp.push_back(ans_r);
            }
            if(!tmp.empty()){
                ans.push_back(tmp);
                ans_name.push_back(pathin);
            }

        }
        
    }
    //计算gap

    std::ofstream outfile(path+"large_ratio.csv");  // 创建并打开CSV文件
    //有解的实例ratio
    if (!outfile) {
        std::cerr << "无法打开文件！" << std::endl;
        return 1;
    }
    //一行表示一个实例的各种算法的ratio，前提是ILP存在解。
    for(int i=0;i<ans.size();i++){
        outfile<<ans_name[i]<<",";
        for(int j=0;j<ans[i].size();j++){
            outfile<<ans[i][j]<<",";
        }
        outfile<<endl;
    }

    outfile.close();  // 关闭文件
}




int main(){
    //auto l=small_ratio();
    auto l1=large_ratio();
    return 0;
    

}