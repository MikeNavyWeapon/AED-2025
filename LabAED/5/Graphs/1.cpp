#include <stdio.h>
void BFS(int G[7][7], int start) {
    int i=start,j;
    int visited[7]={0};
    printf("%d ",i);
    visited[i]=1;
    enqueue(i);

    while(!isEmpty()) {
        i=dequeue();
        for(j=0;j<7;j++) {
            if(G[i][j]==1 && visited[j]==0) {
                printf("%d ",j);
                visited[j]=1;
                enqueue(j);
            }
        }
    }
}

void DFS(int G[7][7], int start) {
    int i=start,j;
    int visited[7]={0};
    printf("%d ",i);
    visited[i]=1;
    push(i);

    while(!isEmpty()) {
        i=pop();
        for(j=0;j<7;j++) {
            if(G[i][j]==1 && visited[j]==0) {
                printf("%d ",j);
                visited[j]=1;
                push(j);
            }
        }
    }
}
int main(){
    int G[7][7]={{0,1,0,0,1,0,0},
                 {1,0,1,1,0,0,0},
                 {0,1,0,1,0,0,0},
                 {0,1,1,0,1,1,0},
                 {1,0,0,1,0,1,1},
                 {0,0,0,1,1,0,1},
                 {0,0,0,0,1,1,0}};
    return 0;
    BFS(G, 4,7);
}