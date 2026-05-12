#include <stdio.h>

struct Node
{
    char data;
    struct Node *next;
} *top=NULL;

}top=NULL;

void push(char x)
{
    struct Node *t;
    t=(struct Node *)malloc(sizeof(struct Node));
    if(t==NULL)
        printf("Stack overflow\n");
    else
    {
        t->data=x;
        t->next=top;
        top=t;
    }
}

char pop()
{
    char x=-1;
    struct Node *t;
    if(top==NULL)
        printf("Stack underflow\n");
    else
    {
        t=top;
        x=t->data;
        top=top->next;
        free(t);
    }
    return x;
}

void Display()
{
    struct Node *p=top;
    while(p!=NULL)
    {
        printf("%d ", p->data);
        p=p->next;
    }
    printf("\n");
}

int isBalanced(char *exp)
{
    for(int i=0;exp[i]!='\0';i++)
    {
         if(exp[i]=='(')
              push(exp[i]);
         else if(exp[i]==')')
         {
              if(top==NULL)
              {
                printf("Invalid expression\n");
                return 0;
              }
              pop();
         }
    }
    if(top==NULL)
        printf("Valid expression\n");
    else
        printf("Invalid expression\n");
}

int pre(char x)
{
    if(x=='+' || x=='-')
        return 1;
    else if(x=='*' || x=='/')
        return 2;
    return 0;
}

int isOperand(char x)
{
    if(x>='0' && x<='9' || x>='a' && x<='z' || x>='A' && x<='Z')
        return 1;
    return 0;
}

char *InToPost(char *infix)
{
    char *postfix;
    int i=0,j=0;
    int len=strlen(infix);
    postfix=(char *)malloc((len+1)*sizeof(char));
    while(infix[i]!='\0')
    {
        if(isOperand(infix[i]))
            postfix[j++]=infix[i++];
        else
        {
            if(pre(infix[i])>pre(top->data))
                push(infix[i++]);
            else
                postfix[j++]=pop();
        }
    }
    postfix[j]='\0';
    return postfix;
}

int Eval(char *postfix)
{
    int i=0,x1,x2,r;
    while(postfix[i]!='\0')
    {
        if(isOperand(postfix[i]))
            push(postfix[i++]-'0');
        else
        {
            x2=pop();
            x1=pop();
            switch(postfix[i++])
            {
                case '+': r=x1+x2; break;
                case '-': r=x1-x2; break;
                case '*': r=x1*x2; break;
                case '/': r=x1/x2; break;
            }
            push(r);
        }
    }
    return pop();
}

int main()
{
   char *infix="a+b*c";
   push('#');

   char *postfix=InToPost(infix);
    printf("%s\n", postfix);
}   