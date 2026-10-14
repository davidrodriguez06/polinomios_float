#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANIO 10

// Nodo para la lista enlazada (manejo de colisiones)
typedef struct HashNode {
    char *clave;
  //  int valor;
    struct ListValor *lv; //aqui guardare los valores repetidos durante una colision
    struct HashNode *siguiente;
} HashNode;


//-----estructura para almacenar los valores-----
typedef struct Valor {
    char valor;
    char clave;
    struct Valor *siguiente;
} Valor;
//------Lista de valores-------
typedef struct ListValor {
    Valor *tabla[TAMANIO];
    int n;
} ListValor;


// Estructura de la tabla hash
typedef struct {
    HashNode *tabla[TAMANIO];
} HashTable;

unsigned int funcion_hash(const char *clave) {
    unsigned int hash = 0;
    while (*clave) {
        hash = (hash * 31) + *clave;
        clave++;
    }
    return hash % TAMANIO;
}

// Insertar un elemento
void hash_insertar(HashTable *ht, const char *clave, int valor) {
    unsigned int indice = funcion_hash(clave);
    HashNode *actual = ht->tabla[indice];
    HashNode *nuevo = (HashNode *)malloc(sizeof(HashNode));
    ListValor *listaV = (ListValor *)calloc(1,sizeof(ListValor));
    Valor *v = (Valor *)malloc(sizeof(Valor));

    // Verificar si la clave ya existe para actualizarla
    while (actual != NULL) {
        if (strcmp(actual->clave, clave) == 0) {            
            v->valor = valor;
            v->clave = *clave;
            listaV->tabla[listaV->n++] = v;
            actual->lv = listaV;
          //  actual->valor = valor;
            return;
        }
        actual = actual->siguiente;
    }

    // Crear nuevo nodo si no existe
    
   // nuevo->clave = strdup(clave);
    //nuevo->valor = valor;
    nuevo->siguiente = ht->tabla[indice];
    v->valor = valor;
    listaV->tabla[listaV->n++] = v;
    nuevo->clave = strdup(clave);
    nuevo->lv = listaV;
    ht->tabla[indice] = nuevo;
}

// Buscar un elemento
int hash_buscar(HashTable *ht, const char *clave, int *encontrado, ListValor *lv) {
    unsigned int indice = funcion_hash(clave);
    HashNode *actual = ht->tabla[indice];

    while (actual != NULL) {
        if (strcmp(actual->clave, clave) == 0) {
            *encontrado = 1;
            *lv = *actual->lv;
            //return actual->valor;
            return 1;

        }
        actual = actual->siguiente;
    }

    *encontrado = 0;
    return -1;
}

// Borrar una clave específica de la tabla
int hash_borrar(HashTable *ht, const char *clave) {
    unsigned int indice = funcion_hash(clave);
    HashNode *actual = ht->tabla[indice];
    HashNode *anterior = NULL;

    while (actual != NULL) {
        if (strcmp(actual->clave, clave) == 0) {
            // Si el nodo a borrar es el primero de la lista
            if (anterior == NULL) {
                ht->tabla[indice] = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }

            // Liberar memoria del nodo
            free(actual->clave); // Liberar la copia de la cadena creada con strdup
            free(actual);
            return 1; // Borrado con éxito
        }
        anterior = actual;
        actual = actual->siguiente;
    }
    return 0; // Clave no encontrada
}

// Liberar toda la memoria de la tabla hash
void hash_destruir(HashTable *ht) {
    for (int i = 0; i < TAMANIO; i++) {
        HashNode *actual = ht->tabla[i];
        while (actual != NULL) {
            HashNode *temporal = actual;
            actual = actual->siguiente;
            
            free(temporal->clave); // Liberar la clave
            free(temporal);       // Liberar el nodo
        }
        ht->tabla[i] = NULL;
    }
}



int main(){

HashTable HT;
ListValor lv;

//hash_insertar(HashTable *ht, const char *clave, int valor)
hash_insertar(&HT,"Ernesto",10);
hash_insertar(&HT,"Braulio",15);
hash_insertar(&HT,"Catalina",56);
hash_insertar(&HT,"Alondra",18);
hash_insertar(&HT,"Freddy",59);
hash_insertar(&HT,"Ernesto",18);

//hash_buscar(HashTable *ht, const char *clave, int *encontrado)

int encontrado;

//printf("\n %d %d \n",hash_buscar(&HT,"Ernesto",&encontrado),hash_buscar(&HT,"Alondra",&encontrado));
printf("\n %d \n",hash_buscar(&HT,"Ernesto",&encontrado, &lv));

return 0;

}

