#include <stdio.h>
struct Term
{
    int coeff;
    int exp;
};
struct Poly
{
    int n;
    struct Term *t;
};

void create(struct Poly *p)
{
    printf("Enter the number of terms: ");
    scanf("%d",&p->n);
    p->t=(struct Term*)malloc(p->n*sizeof(struct Term));
    printf("Enter the terms: \n");
    for(int i=0;i<p->n;i++)
    {
        scanf("%d%d",&p->t[i].coeff,&p->t[i].exp);
    }
}
void Display(struct Poly p)
{
    for(int i=0;i<p.n;i++)
    {
        printf("%dx^%d ",p.t[i].coeff,p.t[i].exp);
        if(i<p.n-1)
            printf("+ ");
    }
    printf("\n");
}
struct Poly *add(struct Poly *p1, struct Poly *p2)
{
    struct Poly *sum=(struct Poly*)malloc(sizeof(struct Poly));
    sum->n=p1->n+p2->n;
    sum->t=(struct Term*)malloc(sum->n*sizeof(struct Term));
    int i=0, j=0, k=0;
    while(i<p1->n && j<p2->n)
    {
        if(p1->t[i].exp>p2->t[j].exp)
        {
            sum->t[k++]=p1->t[i++];
        }
        else if(p1->t[i].exp<p2->t[j].exp)
        {
            sum->t[k++]=p2->t[j++];
        }
        else
        {
            sum->t[k].coeff=p1->t[i].coeff+p2->t[j].coeff;
            sum->t[k++].exp=p1->t[i++].exp;
        }
    }
    for(;i<p1->n;i++)
        sum->t[k++]=p1->t[i];
    for(;j<p2->n;j++)
        sum->t[k++]=p2->t[j];
    sum->n=k;
    return sum;
}
int main()
{
    struct Poly p1, p2, *p3;
    create(&p1);
    create(&p2);
    p3=add(&p1, &p2);
    printf("The first polynomial is: \n");
    Display(p1);
    printf("The second polynomial is: \n");
    Display(p2);
    printf("The sum of the two polynomials is: \n");
    Display(*p3);
    return 0;
}
