#include <stdio.h>
#include <algorithm>
#include <string.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

#define STACKSIZE 100000000
#define LIT 0
#define LOD 1
#define STO 2
#define CAL 3
#define INT 4
#define JMP 5
#define JPC 6
#define OPR 7
#define OPRF 8
#define LODA 9
#define STOA 10
#define ITF 11
#define FTI 12

struct instruction{
    int f,l,a;
};

instruction code[STACKSIZE];
int code_cnt;
int s[STACKSIZE];

int base(int delta, int *s, int b){
    while(delta--){
        b = s[b];
    }return b;
}


void interpret() 
{
    int p, b, t;
    instruction i;  //instruction （指令）的类型定义（包括三个域f,l,a），请自行加入到头文件中，供其他文件共享

    printf("start pl0\n");
    t=0; b=1;  //t：数据栈顶指针；b：基地址；
    p=0;	// 指令指针
    s[1]=0; s[2]=0; s[3]=0;
    do {
//        for(int i = 1; i <= t; i++){
//            fprintf(stderr, "%d ", s[i]);
//        }fprintf(stderr, "\n");
//        fprintf(stderr, "%d %d %d %d\n", p, code[p].f, code[p].l, code[p].a);
        i=code[p++];//code为指令存放数组，其定义请自行加入到头文件中，供其他文件共享
        switch (i.f) 
        {
        case LIT: 
            t=t+1;
            s[t]=i.a;
            break;
        case OPR: 
            switch(i.a) 
            {
                case 0:
                    t=b-1;
                    p=s[t+3];
                    b=s[t+2];
                    break;
                case 1: 
                    s[t]=-s[t];
                    break;
                case 2: 
                    t=t-1;
                    s[t]=s[t] + s[t+1];
                    break;
                case 3:
                    t=t-1;
                    s[t]=s[t] - s[t+1];
                    break;
                case 4: 
                    t=t-1;
                    s[t]=s[t] * s[t+1];
                    break;
                case 5: 
                    t=t-1;
                    s[t]=s[t] / s[t+1];
                    break;
                case 6: 
                    s[t]=(s[t] % 2 == 1);
                    break;
                case 8: 
                    t=t-1;
					
                    s[t]=(s[t] == s[t+1]);
                    break;
                case 9:
                    t=t-1;
                    s[t]=(s[t] != s[t+1]);
                    break;
                case 10:
                    t=t-1;
                    s[t]=(s[t]<s[t+1]);
                    break;
                case 11: 
                    t=t-1;
                    s[t]=(s[t]>=s[t+1]);
                    break;
                case 12: 
                    t=t-1;
                    s[t]=(s[t]>s[t+1]);
                    break;
                case 13: 
                    t=t-1;
                    s[t]=(s[t]<=s[t+1]);
                    break;
                case 14: 
                    printf(" %d", s[t]);
                    t=t-1;
                    break;
                case 15: 
                    printf("\n");
                    break;
                case 16: 
                    t=t+1;
                    printf(" ? ");
                    scanf("%d", &s[t]);
                    break;
                case 17:
                    t=t-2;
                    s[t]=s[t]*s[t+2]+s[t+1];
            }
		    break;
        case LOD: 
            t=t+1;
            s[t]=s[base(i.l, s, b)+i.a];
            break;
        case LODA:
            s[t]=s[base(i.l, s, b)+i.a+s[t]];
            break;
        case STO: 
            s[base(i.l, s, b)+i.a]=s[t];
            t=t-1;
            break;
        case STOA:
            t=t-2;
            s[base(i.l, s, b)+i.a+s[t+1]]=s[t+2];
            break;
        case CAL:
            s[t+1]=base(i.l, s, b);
            s[t+2]=b;
            s[t+3]=p;
            b=t+1;
            p=i.a;
            break;
        case INT: 
            t=t+i.a;
            break;
        case JMP: 
            p=i.a;
            break;
        case JPC: 
            if (s[t]==0) 
            {
                p=i.a;
            }
			//printf("---nextp %d  ----\n",p);
            t=t-1;
            break;
        }//end switch
    }while (p!=0);
}

int main(int argc, char **argv){
    char buf[10];
    int l, a;
    FILE *in = fopen(argv[1], "r");
    while(fscanf(in, "%s %d %d\n", buf, &l, &a) != EOF){
        if(!strcmp(buf, "lit")) code[code_cnt++] = (instruction){LIT, l, a};
        if(!strcmp(buf, "lod")) code[code_cnt++] = (instruction){LOD, l, a};
        if(!strcmp(buf, "sto")) code[code_cnt++] = (instruction){STO, l, a};
        if(!strcmp(buf, "cal")) code[code_cnt++] = (instruction){CAL, l, a};
        if(!strcmp(buf, "int")) code[code_cnt++] = (instruction){INT, l, a};
        if(!strcmp(buf, "jmp")) code[code_cnt++] = (instruction){JMP, l, a};
        if(!strcmp(buf, "jpc")) code[code_cnt++] = (instruction){JPC, l, a};
        if(!strcmp(buf, "opr")) code[code_cnt++] = (instruction){OPR, l, a};
        if(!strcmp(buf, "loda")) code[code_cnt++] = (instruction){LODA, l, a};
        if(!strcmp(buf, "stoa")) code[code_cnt++] = (instruction){STOA, l, a};
    }interpret();
}
