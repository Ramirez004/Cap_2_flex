#include <stdio.h>

extern int yylex(void);
extern int yylineno;
extern char *curfilename;

void printrefs(void);

int main(int argc, char **argv)
{
    int i;

    if(argc < 2) {
        curfilename = "(stdin)";
        yylineno = 1;
        yylex();
    } 
    else {
        for(i = 1; i < argc; i++) {

            FILE *f = fopen(argv[i], "r");

            if(!f) {
                perror(argv[i]);
                return 1;
            }

            curfilename = argv[i];
            yylineno = 1;

            yyrestart(f);
            yylex();

            fclose(f);
        }
    }

    printrefs();

    return 0;
}
