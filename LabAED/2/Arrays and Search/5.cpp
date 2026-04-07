#include <stdio.h>
#include <stdlib.h>

struct Array
{
    int *A;
    int size;
    int length; 
};
void Display(struct Array arr)
{
    int i;
    printf("\nElements are: \n");
    for(i=0;i<arr.length;i++)
    {
        printf("%d ", arr.A[i]);
    }
}

int main()
{
    struct Array arr;
    int n,i;
    printf("Enter size of array: ");
    if (scanf("%d", &arr.size) != 1 || arr.size <= 0)
    {
        printf("Invalid size.\n");
        return 1;
    }
    arr.A = (int *)malloc(arr.size * sizeof(int));
    arr.length = 0;

    printf("Enter number of elements: ");
    if (scanf("%d", &n) != 1 || n < 0 || n > arr.size)
    {
        printf("Invalid number of elements.\n");
        free(arr.A);
        return 1;
    }

    printf("Enter all the elements: \n");
    for(i=0;i<n;i++)
    {
        if (scanf("%d", &arr.A[i]) != 1)
        {
            printf("Invalid element input.\n");
            free(arr.A);
            return 1;
        }
    }
    arr.length = n;
    Display(arr);

    free(arr.A);
    return 0;
}
