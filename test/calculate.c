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

int add(int a, int b) {
    int ret;
    ret = a + b;
    return ret;
}
int sub(int a, int b) {
    int ret;
    ret = a - b;
    return ret;
}
int mult(int a, int b) {
    int ret;
    ret = a * b;
    return ret;
}
int div(int a, int b) {
    int ret;
    ret = a / b;
    return ret;
}
int mod(int a, int b) {
    int ret;
    ret = a % b;
    return ret;
}
int main() {
    printf("%d + %d = %d\n", 37, 9, add(37, 9));
    printf("%d - %d = %d\n", 37, 9, sub(37, 9));
    printf("%d x %d = %d\n", 37, 9, mult(37, 9));
    printf("%d / %d = %d\n", 37, 9, div(37, 9));
    printf("%d %% %d = %d\n", 37, 9, mod(37, 9));
    return 0;
}