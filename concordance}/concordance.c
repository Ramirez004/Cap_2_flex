#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NHASH 9997

struct ref {
    char *filename;
    int lineno;
    struct ref *next;
};

struct symbol {
    char *name;
    struct ref *reflist;
};

struct symbol symtab[NHASH];

unsigned symhash(char *sym)
{
    unsigned int hash = 0;
    unsigned c;

    while((c = *sym++))
        hash = hash * 31 + tolower(c);

    return hash % NHASH;
}

struct symbol *lookup(char *sym)
{
    struct symbol *sp = &symtab[symhash(sym)];

    if(!sp->name) {
        sp->name = strdup(sym);
        sp->reflist = NULL;
    }

    return sp;
}

void addref(char *word, char *filename, int lineno)
{
    struct ref *r;
    struct symbol *sp = lookup(word);

    r = malloc(sizeof(struct ref));

    r->filename = strdup(filename);
    r->lineno = lineno;
    r->next = sp->reflist;

    sp->reflist = r;
}

void printrefs(void)
{
    int i;
    struct ref *r;

    for(i = 0; i < NHASH; i++) {
        if(symtab[i].name) {
            printf("%s:\n", symtab[i].name);

            for(r = symtab[i].reflist; r; r = r->next)
                printf("   %s:%d\n", r->filename, r->lineno);
        }
    }
}
