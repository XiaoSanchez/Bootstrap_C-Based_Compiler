/*
 * @Description: 
 * @Logo:                                                               ▄   ▄
 * ░██████╗██╗░░██╗░█████╗░░██╗░░░░░░░██╗███╗░░██╗  ░█████╗░░█████╗░██╗ █▀█▀█
 * ██╔════╝██║░░██║██╔══██╗░██║░░██╗░░██║████╗░██║  ██╔══██╗██╔══██╗██║ █▄█▄█
 * ╚█████╗░███████║███████║░╚██╗████╗██╔╝██╔██╗██║  ██║░░╚═╝███████║██║ ███  ▄▄
 * ░╚═══██╗██╔══██║██╔══██║░░████╔═████║░██║╚████║  ██║░░██╗██╔══██║██║ ████▐█ █
 * ██████╔╝██║░░██║██║░░██║░░╚██╔╝░╚██╔╝░██║░╚███║  ╚█████╔╝██║░░██║██║ ████   █
 * ╚═════╝░╚═╝░░╚═╝╚═╝░░╚═╝░░░╚═╝░░░╚═╝░░╚═╝░░╚══╝  ░╚════╝░╚═╝░░╚═╝╚═╝ ▀▀▀▀▀▀▀
 * @Author: Shawn C
 * Copyright (c) 2022 by Shawn C., All Rights Reserved. 
 */
#include <stdio.h>
int insert(int a[], int n, int key, int capacity) {
    if (n >= capacity)
        return n;
    a[n] = key;
    return (n + 1);
}

int main() {
    int a[20] = {1, 2, 9, 8, 5, 7};
    int capacity = sizeof(a) / sizeof(a[0]);
    int n = 6;
    int i, key = 6;
    printf("To insert the %d to the list of items.", key);
    printf("\n The Origin List: ");
    for (i = 0; i < n; i++)
        printf("%d  ", a[i]);
 
    n = insert(a, n, key, capacity);
 
    printf("\n After Insertion: ");
    for (i = 0; i < n; i++)
        printf("%d  ", a[i]);
 
    return 0;
}