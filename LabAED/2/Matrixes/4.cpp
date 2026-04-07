#include <stdio.h>

struct Element
{
    int i;
    int j;
    int x;
};

struct Sparse
{
    int m;
    int n;
    int num;
    struct Element *ele;
};

void create(struct Sparse *s)
{
    printf("Enter the dimensions of the matrix: ");
    scanf("%d%d",&s->m,&s->n);
    printf("Enter the number of non-zero elements: ");
    scanf("%d",&s->num);
    s->e=(struct Element*)malloc(s->num*sizeof(struct Element));
    printf("Enter the non-zero elements: \n");
    for(int i=0;i<s->num;i++)
    {
        scanf("%d%d%d",&s->e[i].i,&s->e[i].j,&s->e[i].x);
    }
}

void Display(struct Sparse s)
{
    int i,j,k=0;
    for(i=1;i<=s.m;i++)
    {
        for(j=1;j<=s.n;j++)
        {
            if(k<s.num && s.e[k].i==i && s.e[k].j==j)
                printf("%d ",s.e[k++].x);
            else
                printf("0 ");
        }
        printf("\n");
    }
}

add(strcut Sparse *s1,strcut Sparse *s2)
{
    struct Sparse *sum;
    sum(struct Sparse *)malloc(sizeof(struct Sparse));
    int i, j, k;
    i=j=k=0;

    sum=(struct Sparse *)malloc(sizeof(struct Sparse));
    sum->ele=(struct Element *)malloc((s1->num+s2->num)*sizeof(struct Element));

    while(i<s1->num && j<s2->num)
    {
        if(s1->ele[i].i<s2->ele[j].i || (s1->ele[i].i==s2->ele[j].i && s1->ele[i].j<s2->ele[j].j))
            sum->ele[k++]=s1->ele[i++];
        else if(s1->ele[i].i>s2->ele[j].i || (s1->ele[i].i==s2->ele[j].i && s1->ele[i].j>s2->ele[j].j))
            sum->ele[k++]=s2->ele[j++];
        else
        {
            if (s1->ele[i].j<s2->ele[j].j)
                sum->ele[k++]=s1->ele[i++];
            else if(s1->ele[i].j>s2->ele[j].j)
                sum->ele[k++]=s2->ele[j++];
            else
            {
                sum->ele[k]=s1->ele[i];
                sum->ele[k++].x=s1->ele[i++].x+s2->ele[j++].x;
            }
        }
    }
    for (;i<s1->num;i++)
                sum->ele[k++]=s1->ele[i];
            for (;j<s2->num;j++) 
                sum->ele[k++]=s2->ele[j];
}

int main()
{
    struct Sparse s1, s2, *s3;
    create(&s1);
    create(&s2);
    s3=add(&s1, &s2);
    printf("The sum of the two sparse matrices is: \n");
    Display(*s3);
    return 0;   
    
}