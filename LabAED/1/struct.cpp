#include <stdio.h>

struct Rectangle
{
    int length;
    int breadth;
    char x;
};

int main()
{
    struct Rectangle r1={10,5};

    r1.length = 15;
    r1.breadth = 7;

    printf("%d\n", r1.length);
    printf("%d\n", r1.breadth);

    return 0;

}
