
#include <stdio.h>
int findElement(int a[], int n, int key);
int deleteElement(int a[], int n, int key) {
    int pos = findElement(a, n, key);
    if (pos == -1) {
        printf("\n Error: Element not found");
        return n;
    }
    int i;
    for (i = pos; i < n - 1; i++)
        a[i] = a[i + 1];
    return n - 1;
}
int findElement(int a[], int n, int key) {
    int i;
    for (i = 0; i < n; i++)
        if (a[i] == key)
            return i;
 
    return -1;
}
int main() {
    int i;
    int a[] = {1, 5, 3, 4, 2};
 
    int n = sizeof(a) / sizeof(a[0]);
    int key = 3;
    printf("To remove the %d from the list of items.", key);
    printf("\nOriginal List: ");
    for (i = 0; i < n; i++)
        printf("%d  ", a[i]);
     
    n = deleteElement(a, n, key);
 
    printf("\nAfter Deleted: ");
    for (i = 0; i < n; i++)
        printf("%d  ", a[i]);
 
    return 0;
}