#include <stdio.h>

struct Matrix
{
    int A[10];
    int n;
};

void Set(struct Matrix *m,int i,int j,int x)
{
    if(i>=j)
        m->A[i*(i-1)/2+j-1]=x;
}



void Get (struct Matrix *m,int i,int j)
{
    if(i>=j)
        return m.A[i*(i-1)/2+j-1];
    else
        return 0;
}

void Display(struct Matrix m)
{
    int i,j;
    for(i=1;i<=m.n;i++)
    {
        for(j=1;j<=m.n;j++)
        {
            if(i>=j)
                printf("%d ",Get(m,i,j));
            else
                printf("0 ");
        }
        printf("\n");
    }
}



int main()
{
    struct Matrix m;
    printf("Enter the size of the matrix: ");
    scanf("%d",&m.n);
    m.A=(int*)malloc(m.n*sizeof(int));

    printf("Enter the elements of the matrix: \n");
    for(int i=1;i<=m.n;i++)
    {
        for(int j=1;j<=m.n;j++)
        {
            int x;
            scanf("%d",&x);
            Set(&m,i,j,x);
        }
    }   
    printf("The matrix is: \n");
    Display(m);
}