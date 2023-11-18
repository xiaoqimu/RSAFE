#include<bits/stdc++.h>
#include <climits>
#include <cstdint>
#include <utility>
#include <vector>

using namespace std;
template <typename T>
void myprint(T& t){
    for(auto& i:t){
        cout<<i<<" ";
    }
}

int main(){
    freopen("4.out_muti","r",stdin);
    int addEdgeNum;
    scanf("%d",&addEdgeNum);

    freopen("4.in_muti","r",stdin);
    int N,M,T,P,D; //�ڵ���������������ҵ��������ͨ��������˥������
    scanf("%d%d%d%d%d",&N,&M,&T,&P,&D);
    //int edge[25000][83]; //��
    int edge[M+addEdgeNum][3]; //��
    map<pair<int, int>, int> minEdge; //�����ڵ�����бߵ���̾���
   // bool f_lianxu=true;//连续
    bool f_lianxu=false;//不连续
    for(int i = 0;i < M;i++){
        scanf("%d%d%d",&edge[i][0],&edge[i][1],&edge[i][2]);
        pair<int, int> order = make_pair(min(edge[i][0],edge[i][1]), max(edge[i][1],edge[i][0]));
        if(minEdge.count(order)){
            minEdge[order] = min(minEdge[order],edge[i][2]);
        }else minEdge[order] = edge[i][2];
    }

    int task[10000][3]; //业务起点 终点 和需要波长数量
    for(int i = 0;i < T;i++){
        scanf("%d%d%d",&task[i][0],&task[i][1],&task[i][2]);
    }
    
    freopen("4.out_muti","r",stdin);
    int addNum;
    scanf("%d",&addNum);
    if(addNum > 20000) {
        printf("error1: Exceeding the edge limit.\n");
        return 0;
    }
    for(int i = M;i < M + addNum;i++){
        scanf("%d%d",&edge[i][0],&edge[i][1]);
        if(edge[i][0] >=N || edge[i][1] >= N) {
            printf("error7: Illegal addition of edges.\n");
            printf("error2: edge%d Illegal point number.\n",i);
            return 0;
        }
        pair<int, int> order = make_pair(min(edge[i][0],edge[i][1]), max(edge[i][1],edge[i][0]));
        edge[i][2] = minEdge[order];
    }

    vector<int> result_bochang[10010];
    vector<int> result_luyou[10010];
    vector<int> result_fd[10010];
    vector<vector<int>> channel(M+addEdgeNum,vector<int>(P,0));//0未使用，1使用
    for (int i = 0; i < T; i++){
        vector<int> temp(task[i][2]);
        for(int j=0;j<temp.size();j++){
            scanf("%d",&temp[j]);
            if(!(temp[j] >= 0 && temp[j] < P)) {
                printf("error4: task%d Illegal channel number.\n", i);
                return 0;
            }

        }
        if(f_lianxu){
            int pre_j=temp[0];
            for(int j=1;j<temp.size();j++){
                if((pre_j+1)==temp[j]){
                    pre_j++;
                }
                else{
                    cout<<"不连续"<<endl;
                    return 0;
                }
            }
        }

        result_bochang[i]=temp;
        int size_lu=0;
        int size_fd=0;
        scanf("%d %d",&size_lu,&size_fd);
        if(size_lu != 0){
        if(size_lu < 0 || size_lu > 25000) {
            printf("error5: task%d Illegal number of path edges.\n", i);
            return 0;
        }
        if(size_fd < 0 || size_fd > size_lu-1) {
            printf("error6: task%d Illegal number of amplifiers.\n", i);
            return 0;
        }
        }
        vector<int> temp_lu(size_lu);
        vector<int> temp_fd(size_fd);
        for(int j=0;j<temp_lu.size();j++){
            scanf("%d",&temp_lu[j]);
            if(temp_lu[j] >= M + addNum) {
                printf("error3: task%d edge%d Illegal edge number.\n", i, j);
                return 0;
            }
        }
        result_luyou[i]=temp_lu;
        for(int j=0;j<temp_fd.size();j++){
            scanf("%d",&temp_fd[j]);
        }
        result_fd[i]=temp_fd;
        // myprint(result_bochang[i]);
        // cout<<result_luyou[i].size()<<" "<<result_fd[i].size()<<" ";
        // myprint(result_luyou[i]);
        // myprint(result_fd[i]);
        // cout<<endl;
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
        preNode = task[i][0];
        int sum = 0;
        for(int k=0;k<result_luyou[i].size();k++){//每一条路径
            for(int j=0;j<result_fd[i].size();j++){//放大器
                if(result_fd[i][j]==preNode){
                    if(sum > D){
                        printf("error10: task%d big%d The optical signal was not amplified in a timely manner.\n", i, j);
                        return 0;
                    }
                    sum = 0;
                }
            }
            sum += edge[result_luyou[i][k]][2];
            if(edge[result_luyou[i][k]][0] == preNode){
                preNode = edge[result_luyou[i][k]][1];
            }else if(edge[result_luyou[i][k]][1] == preNode){
                preNode = edge[result_luyou[i][k]][0];
            }
        }
        if(sum>D){  
            printf("error9: task%d last big Optical signal not available.\n", i);
            return 0;
        }
    }
    
    printf("check end!\n");

    // printf("%d %d %d %d %d\n", N,M,T,P,D);
    // for (int i = 0; i < M + addNum; i++){
    //     printf("%d %d %d\n", edge[i][0],edge[i][1],edge[i][2]);
    // }
    // for (int i = 0; i < T; i++){
    //     printf("%d %d\n", task[i][0],task[i][1]);
    // }
    // for (int i = 0; i < T; i++){
    //     for (int j = 0; j < (3 + result[i][1] + result[i][2]); j++){
    //         printf("%d ", result[i][j]);
    //     }
    //     printf("\n");
    // } 

    return 0;
}