/*************************************************************************
  > File Name: mgmt_client.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 29 Sep 2022 06:43:19 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>


void print_usage() {
    printf("Usage: rectangle [ap] -l num -b num\n");
}

int main(int argc, char *argv[]) {
    int option = 0;
    int area = -1, perimeter = -1, breadth = -1, length =-1;

    //Specifying the expected options
    //The two options l and b expect numbers as argument
    while ((option = getopt(argc, argv,"apl:b:")) != -1) {
        switch (option) {
             case 'a' : area = 0;
                 break;
             case 'p' : perimeter = 0;
                 break;
             case 'l' : length = atoi(optarg); 
                 break;
             case 'b' : breadth = atoi(optarg);
                 break;
             default: print_usage(); 
                 exit(EXIT_FAILURE);
        }
    }
    if (length == -1 || breadth ==-1) {
        print_usage();
        exit(EXIT_FAILURE);
    }

    // Calculate the area
    if (area == 0) {
        area = length * breadth;
        printf("Area: %d\n",area);
    }

    // Calculate the perimeter
    if (perimeter == 0) {
        perimeter = 2 * (length + breadth);
        printf("Perimeter: %d\n",perimeter);
    }
    return 0;
}