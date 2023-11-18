#include<bits/stdc++.h>
#include <climits>
#include <cstdint>
#include <utility>
#include <vector>

using namespace std;

int main(){
    freopen("4.out","r",stdin);
    int addEdgeNum;
    scanf("%d",&addEdgeNum);

    freopen("4.in","r",stdin);
    int N,M,T,P,D; //�ڵ���������������ҵ��������ͨ��������˥������
    scanf("%d%d%d%d%d",&N,&M,&T,&P,&D);
    //int edge[25000][83]; //��
    int edge[M+addEdgeNum][3]; //��
    map<pair<int, int>, int> minEdge; //�����ڵ�����бߵ���̾���

    for(int i = 0;i < M;i++){
        scanf("%d%d%d",&edge[i][0],&edge[i][1],&edge[i][2]);
        pair<int, int> order = make_pair(min(edge[i][0],edge[i][1]), max(edge[i][1],edge[i][0]));
        if(minEdge.count(order)){
            minEdge[order] = min(minEdge[order],edge[i][2]);
        }else minEdge[order] = edge[i][2];
    }

    int task[10000][2]; //ҵ��
    for(int i = 0;i < T;i++){
        scanf("%d%d",&task[i][0],&task[i][1]);
    }
    
    freopen("4.out","r",stdin);
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

    vector<int> result[10010];
    vector<int> channel[M+addEdgeNum];
    for (int i = 0; i < T; i++){
        for (int j = 0; j < 3; j++){
            int temp;
            scanf("%d", &temp);
            result[i].push_back(temp);
        }
        if(!(result[i][0] >= 0 && result[i][0] < P)) {
            printf("error4: task%d Illegal channel number.\n", i);
            return 0;
        }
        if(result[i][1] != 0){
        if(result[i][1] <= 0 || result[i][1] > 25000) {
            printf("error5: task%d Illegal number of path edges.\n", i);
            return 0;
        }
        if(result[i][2] < 0 || result[i][2] > result[i][1]-1) {
            printf("error6: task%d Illegal number of amplifiers.\n", i);
            return 0;
        }
        }
        int preNode = task[i][0];
        for (int j = 0; j < result[i][1]; j++){
            int temp;
            scanf("%d", &temp);
            result[i].push_back(temp);
            if(temp >= M + addNum) {
                printf("error3: task%d edge%d Illegal edge number.\n", i, j);
                return 0;
            }
            if(edge[result[i][j+3]][0] == preNode){
                preNode = edge[result[i][j+3]][1];
            }else if(edge[result[i][j+3]][1] == preNode){
                preNode = edge[result[i][j+3]][0];
            }else{
                printf("error8: task%d edge%d Path discontinuity.\n", i,j);
                return 0;
            }
            if((j == (result[i][1]-1))&&(preNode != task[i][1])){
                printf("error8: task%d last edge Path discontinuity.\n", i);
                return 0;
            }

            for (int k = 0; k < channel[temp].size(); k++)
            {
               if(channel[temp][k] == result[i][0]){
                    printf("error11: task%d edge%d Channel%d Conflict.\n", i,j,result[i][0]);
                    return 0;
                } 
            } 
            channel[temp].push_back(result[i][0]);
        }

        for (int j = 0; j < result[i][2]; j++){
            int temp;
            scanf("%d", &temp);
            result[i].push_back(temp);
        }
        
        preNode = task[i][0];
        int bigLocation = 3 + result[i][1];
        int sum = 0;
        for (int k = 3; k < result[i][1]+3; k++){
            for(int bigLocation = 3 + result[i][1];bigLocation < 3 + result[i][1] + result[i][2];bigLocation++){
                if(result[i][bigLocation] == preNode){
                    if(sum > D){
                        printf("%d %d %d %d %d", k-3, result[i][bigLocation], preNode, sum, D);
                        printf("error10: task%d big%d The optical signal was not amplified in a timely manner.\n", i, bigLocation-3-result[i][1]);
                        return 0;
                    }
                    sum = 0;
                }
            }
            sum += edge[result[i][k]][2];
            if(edge[result[i][k]][0] == preNode){
                preNode = edge[result[i][k]][1];
            }else if(edge[result[i][k]][1] == preNode){
                preNode = edge[result[i][k]][0];
            }
        }
        if(sum > D){
            printf("error9: task%d big%d Optical signal not available.\n", i, bigLocation-3-result[i][1]);
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