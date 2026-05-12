#include <stdio.h>

void swap(int *x, int *y)
{
    int temp;
    temp=*x;
    *x=*y;
    *y=temp;
}

void Bubble(int A[], int n)
{
    int i, j, temp;
    for(i=0; i<n-1; i++)
    {
        for(j=0; j<n-1-i; j++)
        {
            if(A[j]>A[j+1])
            {
                temp=A[j];
                A[j]=A[j+1];
                A[j+1]=temp;
            }
        }
    }
}

void Insertion(int A[], int n)
{
    int i, j, temp;
    for(i=1; i<n; i++)
    {
        temp=A[i];
        j=i-1;
        while(j>=0 && A[j]>temp)
        {
            A[j+1]=A[j];
            j--;
        }
        A[j+1]=temp;
    }
}
void SelectionSort(int A[], int n)
{
    int i, j, k;
    for(i=0; i<n-1; i++)
    {
        for(j=k=i; j<n; j++)
        {
            if(A[j]<A[k])
            {
                k=j;
            }
        }
        swap(&A[i], &A[k]);
    }
}

int partition(int A[], int l, int h)
{
    int pivot=A[l];
    int i=l, j=h;
    do
    {
        do{i++;} while(A[i]<=pivot);
        do{j--;} while(A[j]>pivot);
        if(i<j)
        {
            swap(&A[i], &A[j]);
        }
    } while(i<j);
    swap(&A[l], &A[j]);
    return j;
}
void QuickSort(int A[], int l, int h)
{
    if(l<h)
    {
        int j=partition(A, l, h);
        QuickSort(A, l, j);
        QuickSort(A, j+1, h);
    }
}

void Merge(int A[], int l, int mid, int h)
{
    int i=l, j=mid+1, k=l;
    int B[100];
    while(i<=mid && j<=h)
    {
        if(A[i]<A[j])
        {
            B[k++]=A[i++];
        }
        else
        {
            B[k++]=A[j++];
        }
    }
    for(; i<=mid; i++)
    {
        B[k++]=A[i];
    }
    for(; j<=h; j++)
    {
        B[k++]=A[j];
    }
    for(i=l; i<=h; i++)
    {
        A[i]=B[i];
    }
}
void IMErgeSort(int A[], int n)
{
    int p, l, h, mid;
    for(p=2; p<=n; p=p*2)
    {
        for(l=0; l+p-1<n; l=l+p)
        {
            h=l+p-1;
            mid=(l+h)/2;
            Merge(A, l, mid, h);
        }
    }
    if(l+p/2-1<n)
    {
        Merge(A, l, mid, n-1);
    }
}

void MergeSort(int A[], int l, int h)
{
    if(l<h)
    {
        int mid=(l+h)/2;
        MergeSort(A, l, mid);
        MergeSort(A, mid+1, h);
        Merge(A, l, mid, h);
    }
}
int findMax(int A[], int n)
{
    int max=A[0];
    for(int i=1; i<n; i++)
    {
        if(A[i]>max)
        {
            max=A[i];
        }
    }
    return max;
}
void CountSort(int A[], int n)
{
    int max=findMax(A, n);
    int *C=(int *)malloc((max+1)*sizeof(int));
    for(int i=0; i<=max; i++)
    {
        C[i]=0;
    }
    for(int i=0; i<n; i++)
    {
        C[A[i]]++;
    }
    int i=0, j=0;
    while(i<=max)
    {
        if(C[i]>0)
        {
            A[j++]=i;
            C[i]--;
        }
        else
        {
            i++;
        }
    }
}
void ShellSort(int A[], int n)
{
    for(int gap=n/2; gap>0; gap=gap/2)
    {
        for(int i=gap; i<n; i++)
        {
            int temp=A[i];
            int j=i;
            while(j>=gap && A[j-gap]>temp)
            {
                A[j]=A[j-gap];
                j=j-gap;
            }
            A[j]=temp;
        }
    }
}
int main()
{
    int A[]={5, 3, 8, 4, 2};
    int n=sizeof(A)/sizeof(A[0]);
    SelectionSort(A, n);
    for(int i=0; i<n; i++)
    {
        printf("%d ", A[i]);
    }
    return 0;
}