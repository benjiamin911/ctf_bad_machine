#include<stdio.h>
const char* combineString(char* a,char *b){
        int length = sizeof(a)+sizeof(b)+1;
        char* concatstring = (char*)malloc(length*sizeof(char));
        memset(concatstring,0,length);
        strcat(concatstring,a);
        strcat(concatstring,b);
        return concatstring;
}

int main(){
char* a = "111";
int b = 22;
char buf[100];
sprintf(buf,"%d",b);
const char* c = combineString(a,buf);
printf("%sdone\n",c);
return 0;
}

