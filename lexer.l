%{
    #include<stdio.h>
%}

%%
[0-9]+ {printf("num ");}
[A-Za-z][A-Za-z0-9_]* {printf("id ");}
[+-/*()] {printf("%s ", yytext);}
.   
%%

int main(){
    yylex();
    printf("$");
    return 0;
}