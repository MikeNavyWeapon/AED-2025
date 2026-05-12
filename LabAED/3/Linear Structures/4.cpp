#include <stdio.h>

struct Node
{
    int coeff;
    int exp;
    struct Node *next;
}*poly=NULL;

void create()
{
    struct Node *t,*last;
    int num,i;
    printf("Enter the number of terms: ");
    scanf("%d", &num);
    printf("Enter the coefficient and exponent of each term: ");
    for(i=0;i<num;i++)
    {
        t=(struct Node *)malloc(sizeof(struct Node));
        scanf("%d%d", &t->coeff, &t->exp);
        t->next=NULL;
        if(poly==NULL)
            poly=last=t;
        else
        {
            last->next=t;
            last=t;
        }
    }
}

void Display(struct Node *p)
{
    while(p)
    {
        printf("%dx^%d ", p->coeff, p->exp);
        p=p->next;
    }
}

long Eval(struct Node *p, int x)
{
    long val;
    while(p)
    {
        val+=p->coeff*pow(x, p->exp);
        p=p->next;
    }
    return val;
}

int main()
{
    create();
    printf("The polynomial is: ");
    Display(poly);
    printf("%ld\n", Eval(poly,1));
    return 0;
}