#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANIO 10

typedef struct Valor {
    int   valor;
    char *clave;
    struct Valor *siguiente;
} Valor;

typedef struct ListValor {
    Valor *tabla[TAMANIO];
    int    n;
} ListValor;

typedef struct HashNode {
    char *clave;
    ListValor *lv;
    struct HashNode *siguiente;
} HashNode;

typedef struct {
    HashNode *tabla[TAMANIO];
} HashTable;

unsigned int funcion_hash(const char *clave) {
    unsigned int hash = 0;
    while (*clave) {
        hash = (hash * 31) + (unsigned char)*clave;
        clave++;
    }
    return hash % TAMANIO;
}

void hash_insertar(HashTable *ht, const char *clave, int valor) {
    unsigned int indice = funcion_hash(clave);
    HashNode *actual = ht->tabla[indice];
    while (actual != NULL) {
        if (strcmp(actual->clave, clave) == 0) {
            if (actual->lv->n >= TAMANIO) {
                fprintf(stderr, "Lista llena para '%s'\n", clave);
                return;
            }
            Valor *v = malloc(sizeof(Valor));
            v->valor     = valor;
            v->clave     = strdup(clave);
            v->siguiente = NULL;
            actual->lv->tabla[actual->lv->n++] = v;
            return;
        }
        actual = actual->siguiente;
    }

    HashNode *nuevo = malloc(sizeof(HashNode));
    nuevo->clave     = strdup(clave);
    nuevo->lv        = calloc(1, sizeof(ListValor));
    nuevo->siguiente = ht->tabla[indice];

    Valor *v = malloc(sizeof(Valor));
    v->valor     = valor;
    v->clave     = strdup(clave);
    v->siguiente = NULL;
    nuevo->lv->tabla[nuevo->lv->n++] = v;

    ht->tabla[indice] = nuevo;
}

int hash_buscar(HashTable *ht, const char *clave, int *encontrado, ListValor **lv) {
    unsigned int indice = funcion_hash(clave);
    HashNode *actual = ht->tabla[indice];

    while (actual != NULL) {
        if (strcmp(actual->clave, clave) == 0) {
            *encontrado = 1;
            *lv = actual->lv;
            return 1;
        }
        actual = actual->siguiente;
    }

    *encontrado = 0;
    *lv = NULL;
    return 0;
}

static void liberar_lista_valores(ListValor *lv) {
    if (lv == NULL) return;
    for (int i = 0; i < lv->n; i++) {
        free(lv->tabla[i]->clave);
        free(lv->tabla[i]);
    }
    free(lv);
}

int hash_borrar(HashTable *ht, const char *clave) {
    unsigned int indice = funcion_hash(clave);
    HashNode *actual   = ht->tabla[indice];
    HashNode *anterior = NULL;

    while (actual != NULL) {
        if (strcmp(actual->clave, clave) == 0) {
            if (anterior == NULL) {
                ht->tabla[indice] = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            free(actual->clave);
            liberar_lista_valores(actual->lv);
            free(actual);
            return 1;
        }
        anterior = actual;
        actual   = actual->siguiente;
    }
    return 0;
}

void hash_destruir(HashTable *ht) {
    for (int i = 0; i < TAMANIO; i++) {
        HashNode *actual = ht->tabla[i];
        while (actual != NULL) {
            HashNode *temporal = actual;
            actual = actual->siguiente;
            free(temporal->clave);
            liberar_lista_valores(temporal->lv);
            free(temporal);
        }
        ht->tabla[i] = NULL;
    }
}


int main(void) {
    HashTable HT;     

    hash_insertar(&HT, "Ernesto",  10);
    hash_insertar(&HT, "Braulio",  15);
    hash_insertar(&HT, "Catalina", 56);
    hash_insertar(&HT, "Catalina", 18);
    hash_insertar(&HT, "Freddy",   59);
    hash_insertar(&HT, "Ernesto",  18);   

    ListValor *lv = NULL;
    int encontrado = 0;
    char *buscar = "Catalina";

    if (hash_buscar(&HT, buscar, &encontrado, &lv) == 1) {
        printf(buscar,"-> %d valor(es):\n", lv->n);
        for (int i = 0; i < lv->n; i++) {
            printf("  [%d] valor=%d  clave=%s\n",
                   i, lv->tabla[i]->valor, lv->tabla[i]->clave);
        }
    } else {
        printf("No encontrado\n");
    }

    hash_destruir(&HT);
    return 0;
}
