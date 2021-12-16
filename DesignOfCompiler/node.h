#include <cstdio>
#include <iostream>
#include <vector>
#include <string.h>
using namespace std;
struct node {
    char *text;
    vector<node *> ch;
};