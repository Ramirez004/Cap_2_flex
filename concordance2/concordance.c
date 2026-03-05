#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NHASH 9997

extern int yylineno;
extern char *curfilename;

struct ref {
    char *filename;
    int lineno;
    struct ref *next;
};

struct symbol {
    char *name;
    struct ref *reflist;
    struct symbol *next;
};

struct symbol *symtab[NHASH] = {0};   /* tabla inicializada en NULL */

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
    struct symbol *sp;
    int hash = symhash(sym);

    /* buscar en la lista */
    for(sp = symtab[hash]; sp; sp = sp->next) {
        if(strcasecmp(sp->name, sym) == 0)
            return sp;
    }

    /* crear nuevo símbolo */
    sp = malloc(sizeof(struct symbol));

    if(!sp) {
        perror("malloc");
        exit(1);
    }

    sp->name = strdup(sym);
    sp->reflist = NULL;

    /* insertar en la lista */
    sp->next = symtab[hash];
    symtab[hash] = sp;

    return sp;
}

void addref(char *word, char *filename, int lineno)
{
    struct ref *r;
    struct symbol *sp = lookup(word);

    r = malloc(sizeof(struct ref));

    if(!r) {
        perror("malloc");
        exit(1);
    }

    r->filename = strdup(filename);
    r->lineno = lineno;

    r->next = sp->reflist;
    sp->reflist = r;
}

void printrefs(void)
{
    int i;
    struct symbol *sp;
    struct ref *r;

    for(i = 0; i < NHASH; i++) {
        for(sp = symtab[i]; sp; sp = sp->next) {

            printf("%s:\n", sp->name);

            for(r = sp->reflist; r; r = r->next)
                printf("   %s:%d\n", r->filename, r->lineno);
        }
    }
}
