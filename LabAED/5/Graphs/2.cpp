#include <iostream>

int cost[][8]=
{{I,I,I,I,I,I,I,I},
{I,I,25,35,I,I,I,I},
{I,25,I,15,I,I,I,I},
{I,35,15,I,5,I,I,I},
{I,I,I,5,I,10,I,I},
{I,I,I,I,10,I,I,5},
{I,I,I,I,I,I,I,10},
{I,I,I,I,I,I,10,I}};

int near[8]{I,I,I,I,I,I,I,I};
int t[2][7];

int main() {
    int i,j,u,v,n=7;
    for(i=1;i<=n;i++) {
        for(j=1;j<=n;j++) {
            if(cost[i][j]!=I && cost[i][j]!=0) {
                if(near[i]<near[j]) {
                    near[i]=cost[i][j];
                }
                else {
                    near[j]=cost[i][j];
                }
            }
        }
    }
    for(i=1;i<n;i++) {
        int min=I;
        for(j=1;j<=n;j++) {
            if(near[j]!=0 && near[j]<min) {
                min=near[j];
                u=j;
            }
        }
        t[0][i-1]=u;
        t[1][i-1]=near[u];
        near[u]=0;
        for(j=1;j<=n;j++) {
            if(cost[u][j]!=I && near[j]!=0) {
                if(cost[u][j]<near[j]) {
                    near[j]=cost[u][j];
                }
            }
        }
    }
    return 0;
}

