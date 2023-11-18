#include<iostream>
using namespace std;
int main(){
    freopen("4.in_muti","r",stdin);
    freopen("4.in_multi","w",stdout);
    int n,e,k,w;
    cin>>n>>e>>k>>w;
    cout<<n<<" "<<e<<" "<<k<<" "<<w<<endl;
    for(int i=0;i<e;i++){
        int s,e,d;
        cin>>s>>e>>d;
        if(s==e){
            
        }
        cout<<s<<" "<<e<<endl;
    }
    for(int i=0;i<k;i++){
        int s,e,d;
        cin>>s>>e>>d;
        if(s!=e){
            cout<<s<<" "<<e<<" "<<d<<endl;
        }
        else{
            i--;
        }
        
    }
}