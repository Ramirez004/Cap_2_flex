# Cap_2_flex
# Taller Flex – Analizador Léxico y Concordance

Este repositorio contiene la solución a los tres ejercicios propuestos en el taller sobre analizadores léxicos usando **Flex** y el programa **concordance**.

---

# Ejercicio 1

## Pregunta

¿Por qué el programa analiza carácter por carácter en lugar de leer una línea completa usando un patrón como `.*\n`?

## Respuesta

El analizador léxico debe reconocer **patrones específicos dentro del texto**, como por ejemplo:

```
#include
```

Si se utilizara un patrón como:

```
.*\n
```

se consumiría toda la línea como un solo token y el analizador no podría detectar patrones internos.

Por esta razón el programa procesa el texto **carácter por carácter o mediante patrones más específicos**.

Una alternativa válida sería usar:

```
[^\n]*\n
```

que permite leer la línea completa sin interferir con el reconocimiento de patrones definidos.

---

# Ejercicio 2

## Objetivo

Modificar el programa **concordance** para que **no distinga entre mayúsculas y minúsculas**.

Esto significa que palabras como:

```
Casa
CASA
casa
```

deben considerarse **la misma palabra**.

## Cambios realizados

1. Convertir caracteres a minúsculas en la función `symhash()`.
2. Usar `strcasecmp()` en lugar de `strcmp()` para comparar palabras.
3. Incluir la librería `<ctype.h>`.

---

## Archivo `concordance.l`

```c
%{
#include <stdio.h>
#include <ctype.h>
#include <string.h>

extern int yylineno;
char *curfilename;

void addref(char *word, int lineno);
void printrefs();
%}

word    [A-Za-z]+

%%

{word}      { addref(yytext, yylineno); }

\n          { }

.           { }

%%

int yywrap()
{
    return 1;
}
```

---

## Archivo `concordance.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NHASH 9997

typedef struct Ref {
    int lineno;
    struct Ref *next;
} Ref;

typedef struct Symbol {
    char *name;
    Ref *refs;
    struct Symbol *next;
} Symbol;

Symbol *symtab[NHASH];

unsigned symhash(char *sym)
{
    unsigned int hash = 0;
    unsigned c;

    while ((c = *sym++))
        hash = hash * 9 ^ tolower(c);

    return hash;
}

Symbol *lookup(char *sym)
{
    unsigned int h = symhash(sym) % NHASH;
    Symbol *sp;

    for (sp = symtab[h]; sp != NULL; sp = sp->next)
        if (strcasecmp(sp->name, sym) == 0)
            return sp;

    sp = malloc(sizeof(Symbol));
    sp->name = strdup(sym);
    sp->refs = NULL;

    sp->next = symtab[h];
    symtab[h] = sp;

    return sp;
}

void addref(char *word, int lineno)
{
    Symbol *sp = lookup(word);

    Ref *r = malloc(sizeof(Ref));
    r->lineno = lineno;

    r->next = sp->refs;
    sp->refs = r;
}

void printrefs()
{
    int i;
    Symbol *sp;

    for (i = 0; i < NHASH; i++)
    {
        for (sp = symtab[i]; sp != NULL; sp = sp->next)
        {
            printf("%s: ", sp->name);

            Ref *r = sp->refs;
            while (r)
            {
                printf("%d ", r->lineno);
                r = r->next;
            }

            printf("\n");
        }
    }
}
```

---

## Archivo `main.c`

```c
#include <stdio.h>

extern int yylex();
extern int yylineno;
extern FILE *yyin;
extern char *curfilename;

void printrefs();

int main(int argc, char **argv)
{
    int i;

    if (argc < 2)
    {
        curfilename = "(stdin)";
        yylineno = 1;
        yylex();
    }
    else
    {
        for (i = 1; i < argc; i++)
        {
            FILE *f = fopen(argv[i], "r");

            if (!f)
            {
                perror(argv[i]);
                return 1;
            }

            yyin = f;
            curfilename = argv[i];
            yylineno = 1;

            yylex();

            fclose(f);
        }
    }

    printrefs();

    return 0;
}
```

---

# Ejercicio 3

## Problema

La **tabla de símbolos tiene tamaño fijo**, por lo que eventualmente se llena.

El ejercicio pide modificarla para que **pueda crecer**.

## Técnicas propuestas

### 1. Chaining (encadenamiento)

Cada posición de la tabla contiene una **lista enlazada de símbolos**.

Cuando ocurre una colisión:

- el nuevo símbolo se agrega a la lista
- la tabla puede almacenar múltiples elementos por posición

Esto se implementa usando punteros `next`.

### 2. Rehashing

Cuando la tabla se llena:

1. se crea una tabla más grande con `malloc()`
2. se copian los elementos
3. se recalcula el hash de cada símbolo

---

## Técnica utilizada

Se utilizó **Chaining**.

Esto se implementa en la estructura:

```c
typedef struct Symbol {
    char *name;
    Ref *refs;
    struct Symbol *next;
} Symbol;
```

y al insertar:

```c
sp->next = symtab[h];
symtab[h] = sp;
```

---

## Pregunta conceptual

¿Qué técnica hace más difícil generar el cross-reference?

### Respuesta

**Rehashing** hace más difícil producir el cross-reference.

### Razón

Porque cuando la tabla se redimensiona:

- se crea una nueva tabla
- se deben mover todas las entradas
- se recalcula el hash de cada palabra

Esto hace más complejo mantener la estructura de referencias.

En cambio **chaining** simplemente agrega nuevos elementos a las listas enlazadas sin mover los existentes.

---

# Compilación

Para compilar el proyecto:

```bash
flex concordance.l
gcc lex.yy.c concordance.c main.c -o concordance
```

---

# Ejecución

Para analizar un archivo de texto:

```bash
./concordance archivo.txt
```

El programa generará un **cross-reference** indicando en qué líneas aparece cada palabra.

Ejemplo de salida:

```
hola: 1 4 7
mundo: 2 5
programa: 3
```
