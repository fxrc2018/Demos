#include <iostream>
using namespace std;

const int N = 1e6 + 10;

char s1[N];
char s2[N];
int ne[N];

int main(){
    int n,m;
    scanf("%d %s %d %s",&m,s2+1,&n,s1+1);
    // cin>>m>>s2+1>>n>>s1+1;
    
    for(int i=2,j=0;i<=m;i++){
        while(j>0 && s2[i] != s2[j+1]){
            j = ne[j];
        }
        if(s2[i] == s2[j+1]){
            j++;
        }
        ne[i] = j;
    }
    
    for(int i=1,j=0;i<=n;i++){
        while(j>0 && s1[i] != s2[j+1]){
            j = ne[j];
        }
        if(s1[i] == s2[j+1]){
            j++;
        }
        if(j == m){
            printf("%d ",i-m);
        }
    }
    
    return 0;
}