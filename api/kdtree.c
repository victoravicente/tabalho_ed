#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <assert.h>

/*Definições desenvolvedor usuario*/
typedef struct _reg
{
    float embedding[128];
    char id[100];
} treg;

// Função para alocar registro com embedding e id
void *aloca_reg(float embedding[128], const char id[])
{
    treg *reg = malloc(sizeof(treg));
    memcpy(reg->embedding, embedding, sizeof(float) * 128);
    strncpy(reg->id, id, 99);
    reg->id[99] = '\0';
    return reg;
}

// Comparador para KDTree (por dimensão do embedding)
int comparador(void *a, void *b, int pos)
{
    float diff = ((treg *)a)->embedding[pos] - ((treg *)b)->embedding[pos];
    if (diff < 0)
        return -1;
    if (diff > 0)
        return 1;
    return 0;
}

// Distância Euclidiana entre embeddings
double distancia(void *a, void *b)
{
    double soma = 0.0;
    for (int i = 0; i < 128; i++)
    {
        double diff = ((treg *)a)->embedding[i] - ((treg *)b)->embedding[i];
        soma += diff * diff;
    }
    return soma;
}

/*Definições desenvolvedor da biblioteca*/
typedef struct _node
{
    void *key;
    struct _node *esq;
    struct _node *dir;
} tnode;

typedef struct _arv
{
    tnode *raiz;
    int (*cmp)(void *, void *, int);
    double (*dist)(void *, void *);
    int k;
} tarv;

/*funções desenvolvedor da biblioteca*/

void kdtree_constroi(tarv *arv, int (*cmp)(void *a, void *b, int), double (*dist)(void *, void *), int k)
{
    arv->raiz = NULL;
    arv->cmp = cmp;
    arv->dist = dist;
    arv->k = k;
}

/*teste*/
void test_constroi()
{
    /* declaracao de variaveis */
    tarv arv;
    tnode node1;
    tnode node2;

    node1.key = aloca_reg(2, 3, "Dourados");
    node2.key = aloca_reg(1, 1, "Campo Grande");

    /* chamada de funções */
    kdtree_constroi(&arv, comparador, distancia, 2);

    /* testes */
    assert(arv.raiz == NULL);
    assert(arv.k == 2);
    assert(arv.cmp(node1.key, node2.key, 0) == 1);
    assert(arv.cmp(node1.key, node2.key, 1) == 2);
    assert(strcpy(((treg *)node1.key)->nome, "Dourados"));
    assert(strcpy(((treg *)node2.key)->nome, "Campo Grande"));
    free(node1.key);
    free(node2.key);
}

void _kdtree_insere(tnode **raiz, void *key, int (*cmp)(void *a, void *b, int), int profund, int k)
{
    if (*raiz == NULL)
    {
        *raiz = malloc(sizeof(tnode));
        (*raiz)->key = key;
        (*raiz)->esq = NULL;
        (*raiz)->dir = NULL;
    }
    else
    {
        int pos = profund % k;
        if (cmp((*(*raiz)).key, key, pos) < 0)
        {
            _kdtree_insere(&((*(*raiz)).dir), key, cmp, profund + 1, k);
        }
        else
        {
            _kdtree_insere(&((*raiz)->esq), key, cmp, profund + 1, k);
        }
    }
}

void kdtree_insere(tarv *arv, void *key)
{
    _kdtree_insere(&(arv->raiz), key, arv->cmp, 0, arv->k);
}

void _kdtree_destroi(tnode *node)
{
    if (node != NULL)
    {
        _kdtree_destroi(node->esq);
        _kdtree_destroi(node->dir);
        free(node->key);
        free(node);
    }
}

void kdtree_destroi(tarv *arv)
{
    _kdtree_destroi(arv->raiz);
}

void _kdtree_busca(tarv *arv, tnode **atual, void *key, int profund, double *menor_dist, tnode **menor)
{
    tnode **lado_principal;
    tnode **lado_oposto;
    if (*atual != NULL)
    {
        double dist_atual = arv->dist((*atual)->key, key);
        if (dist_atual < *menor_dist)
        {
            *menor_dist = dist_atual;
            *menor = *atual;
        }
        int pos = profund % arv->k;
        int comp = arv->cmp(key, (*atual)->key, pos);

        printf("%s dist %4.3f menor_dist %4.3f comp %d\n", ((treg *)((tnode *)*atual)->key)->nome, dist_atual, *menor_dist, comp);

        /* define lado principal para buscar */
        if (comp < 0)
        {
            lado_principal = &((*atual)->esq);
            lado_oposto = &((*atual)->dir);
        }
        else
        {
            lado_principal = &((*atual)->dir);
            lado_oposto = &((*atual)->esq);
        }

        _kdtree_busca(arv, lado_principal, key, profund + 1, menor_dist, menor);

        /* Verifica se deve buscar também no outro lado*/

        if (comp * comp < *menor_dist)
        {
            printf("tentando do outro lado %f\n", comp * comp);
            _kdtree_busca(arv, lado_oposto, key, profund + 1, menor_dist, menor);
        }
    }
}

tnode *kdtree_busca(tarv *arv, void *key)
{
    tnode *menor = NULL;
    double menor_dist = DBL_MAX;
    _kdtree_busca(arv, &(arv->raiz), key, 0, &menor_dist, &menor);
    return menor;
}

treg buscar_mais_proximo(tarv *arv, treg query)
{
    double menor_dist = 1e20;
    tnode *menor = NULL;
    _kdtree_busca(arv, &(arv->raiz), &query, 0, &menor_dist, &menor);
    return *((treg *)(menor->key));
}

tarv arvore_global;

tarv *get_tree()
{
    return &arvore_global;
}

void inserir_ponto(treg p)
{
    treg *novo = malloc(sizeof(treg));
    *novo = p; // cópia de estrutura
    kdtree_insere(&arvore_global, novo);
}
void kdtree_construir()
{
    arvore_global.k = 2;
    arvore_global.dist = distancia;
    arvore_global.cmp = comparador;
    arvore_global.raiz = NULL;
}

void test_busca()
{
    tarv arv;
    kdtree_constroi(&arv, comparador, distancia, 2);
    kdtree_insere(&arv, aloca_reg(10, 10, "a"));
    kdtree_insere(&arv, aloca_reg(20, 20, "b"));
    kdtree_insere(&arv, aloca_reg(1, 10, "c"));
    kdtree_insere(&arv, aloca_reg(3, 5, "d"));
    kdtree_insere(&arv, aloca_reg(7, 15, "e"));
    kdtree_insere(&arv, aloca_reg(4, 11, "f"));
    tnode *raiz = arv.raiz;
    assert(strcmp(((treg *)raiz->dir->key)->nome, "b") == 0);
    assert(strcmp(((treg *)raiz->esq->key)->nome, "c") == 0);
    assert(strcmp(((treg *)raiz->esq->esq->key)->nome, "d") == 0);
    assert(strcmp(((treg *)raiz->esq->dir->key)->nome, "e") == 0);

    printf("\n");
    treg *atual = aloca_reg(7, 14, "x");
    tnode *mais_proximo = kdtree_busca(&arv, atual);
    assert(strcmp(((treg *)mais_proximo->key)->nome, "e") == 0);

    printf("\n");
    atual->lat = 9;
    atual->lon = 9;
    mais_proximo = kdtree_busca(&arv, atual);
    assert(strcmp(((treg *)mais_proximo->key)->nome, "a") == 0);

    printf("\n");
    atual->lat = 4;
    atual->lon = 5;
    mais_proximo = kdtree_busca(&arv, atual);
    assert(strcmp(((treg *)mais_proximo->key)->nome, "d") == 0);

    printf("\n");
    atual->lat = 4;
    atual->lon = 9;
    mais_proximo = kdtree_busca(&arv, atual);
    assert(strcmp(((treg *)mais_proximo->key)->nome, "f") == 0);

    free(atual);
    kdtree_destroi(&arv);
}

// Estrutura para heap mínimo dos N vizinhos mais próximos
typedef struct
{
    double *distancias;
    treg **vizinhos;
    int capacidade;
    int tamanho;
} heap_vizinhos;

// Funções auxiliares para heap mínimo
void heap_init(heap_vizinhos *heap, int capacidade)
{
    heap->distancias = malloc(sizeof(double) * capacidade);
    heap->vizinhos = malloc(sizeof(treg *) * capacidade);
    heap->capacidade = capacidade;
    heap->tamanho = 0;
}

void heap_free(heap_vizinhos *heap)
{
    free(heap->distancias);
    free(heap->vizinhos);
}

void heap_push(heap_vizinhos *heap, double dist, treg *vizinho)
{
    if (heap->tamanho < heap->capacidade)
    {
        heap->distancias[heap->tamanho] = dist;
        heap->vizinhos[heap->tamanho] = vizinho;
        int i = heap->tamanho;
        while (i > 0 && heap->distancias[i] > heap->distancias[(i - 1) / 2])
        {
            double tmpd = heap->distancias[i];
            treg *tmpv = heap->vizinhos[i];
            heap->distancias[i] = heap->distancias[(i - 1) / 2];
            heap->vizinhos[i] = heap->vizinhos[(i - 1) / 2];
            heap->distancias[(i - 1) / 2] = tmpd;
            heap->vizinhos[(i - 1) / 2] = tmpv;
            i = (i - 1) / 2;
        }
        heap->tamanho++;
    }
    else if (dist < heap->distancias[0])
    {
        heap->distancias[0] = dist;
        heap->vizinhos[0] = vizinho;
        // heapify down
        int i = 0;
        while (1)
        {
            int maior = i;
            int esq = 2 * i + 1, dir = 2 * i + 2;
            if (esq < heap->tamanho && heap->distancias[esq] > heap->distancias[maior])
                maior = esq;
            if (dir < heap->tamanho && heap->distancias[dir] > heap->distancias[maior])
                maior = dir;
            if (maior == i)
                break;
            double tmpd = heap->distancias[i];
            treg *tmpv = heap->vizinhos[i];
            heap->distancias[i] = heap->distancias[maior];
            heap->vizinhos[i] = heap->vizinhos[maior];
            heap->distancias[maior] = tmpd;
            heap->vizinhos[maior] = tmpv;
            i = maior;
        }
    }
}

// Busca N vizinhos mais próximos
void _kdtree_busca_n(tarv *arv, tnode *atual, void *key, int profund, heap_vizinhos *heap)
{
    if (atual == NULL)
        return;
    double dist_atual = arv->dist(atual->key, key);
    heap_push(heap, dist_atual, (treg *)atual->key);

    int pos = profund % arv->k;
    int comp = arv->cmp(key, atual->key, pos);

    tnode *lado_principal = comp < 0 ? atual->esq : atual->dir;
    tnode *lado_oposto = comp < 0 ? atual->dir : atual->esq;

    _kdtree_busca_n(arv, lado_principal, key, profund + 1, heap);

    // Verifica se precisa buscar do outro lado
    float diff = ((treg *)key)->embedding[pos] - ((treg *)atual->key)->embedding[pos];
    if (heap->tamanho < heap->capacidade || diff * diff < heap->distancias[0])
    {
        _kdtree_busca_n(arv, lado_oposto, key, profund + 1, heap);
    }
}

// Função pública para buscar N vizinhos mais próximos
void buscar_n_vizinhos(tarv *arv, treg *query, int n, treg *saida[])
{
    heap_vizinhos heap;
    heap_init(&heap, n);
    _kdtree_busca_n(arv, arv->raiz, query, 0, &heap);
    // Copia os vizinhos encontrados para o vetor de saída
    for (int i = 0; i < heap.tamanho; i++)
    {
        saida[i] = heap.vizinhos[i];
    }
    heap_free(&heap);
}

int main(void)
{
    test_constroi();
    test_busca();
    printf("SUCCESS!!\n");
    return EXIT_SUCCESS;
}
