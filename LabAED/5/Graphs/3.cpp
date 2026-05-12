#include <iostream>

#define I INT_MAX
using namespace std;

void Myunion(int u,int v){
    if (set[u] < set[v]) {
        set[u] += set[v];
        set[v] = u;
    } else {
        set[v] += set[u];
        set[u] = v;
    }
}

int find(int u){
    while(set[u]>0) {
        u=set[u];
    }
    return u;
}
int edges[3][9]={{0,1,2,3,4,5,6,7,8},
              {1,1,2,3,4,5,6,7,8},
              {10,15,5,20,25,30,35,40,45}};
int set[7]={-1,-1,-1,-1,-1,-1,-1};
int included[9]={0,0,0,0,0,0,0,0,0};
int t[2][6];

int main() {
    int i,j,u,v,n=7,e=9;
    for(i=1;i<e;i++) {
        for(j=0;j<e-i-1;j++) {
            if(edges[2][j]>edges[2][j+1]) {
                swap(edges[0][j],edges[0][j+1]);
                swap(edges[1][j],edges[1][j+1]);
                swap(edges[2][j],edges[2][j+1]);
            }
        }
    }
    for(i=0;i<e;i++) {
        u=find(edges[0][i]);
        v=find(edges[1][i]);
        if(u!=v) {
            t[0][i]=edges[2][i];
            t[1][i]=edges[2][i];
            Myunion(u,v);
        }
    }
    return 0;
}