
#  Análise e Documentação do Sistema de Reconhecimento Facial com KDTree

  

##  1. Estrutura do Projeto

  Compilação 
 gcc -c -fpic kdtree.c
 gcc -shared -o libkdtree.so kdtree.o
 
O projeto está organizado da seguinte forma:

- Código C: Implementação da KDTree

- Código Python: API FastAPI e wrappers para interação com a KDTree

- Código de Teste: Scripts para validação do sistema

  

##  2. Análise Comparativa dos Códigos

  

###  2.1 Módulo KDTree (C)

  

####  Alterações Principais:

-  **Estrutura de Dados**:

- Antiga: Usava lat/lon para coordenadas

- Nova: Usa vetor de 128 floats para embeddings faciais

```c

// Antiga

typedef struct _reg {

int lat;

int lon;

char nome[100];

} treg;

  

// Nova

typedef struct _reg {

float embedding[128];

char id[100];

} treg;

```

  

####  Melhorias Implementadas:

1.  **Heap para N Vizinhos**:

- Nova estrutura `heap_vizinhos`

- Funções de gerenciamento do heap

- Busca otimizada para N vizinhos mais próximos

  

###  2.2 API FastAPI (Python)

  

####  Comparativo app.py:

-  **Versão Antiga**:

- Focada em coordenadas geográficas

- Endpoints simples para inserção/busca

-  **Versão Nova**:

- Adaptada para embeddings faciais

- Suporte a busca de múltiplos vizinhos

- Endpoints otimizados para reconhecimento facial

  

###  2.3 Wrapper Python

  

####  Alterações no kdtree_wrapper.py:

- Atualização da estrutura TReg

- Adição de suporte para embeddings

- Melhor integração com a biblioteca C

  

##  3. Funcionalidades Principais

  

###  3.1 Construção da KDTree

```python

@app.post("/construir-arvore")

def constroi_arvore():

lib.kdtree_construir()

```

  

###  3.2 Inserção de Faces

```python

@app.post("/inserir-face")

def inserir_face(dados: EmbeddingEntrada):

# Validação e inserção do embedding

```

  

###  3.3 Busca de Faces Similares

```python

@app.post("/buscar-faces")

async  def buscar_faces(embedding: List[float],  n:  int  =  5):

# Busca dos N vizinhos mais próximos

```

  

##  4. Testes e Validação

  

O sistema inclui:

- Testes unitários em C

- Script de teste Python (test.py)

- Validação com dataset LFW

  

##  5. Conclusão

  

O sistema foi refatorado com sucesso para:

1. Suportar embeddings faciais

2. Implementar busca de N vizinhos

3. Manter compatibilidade com a API existente

4. Adicionar funcionalidades de teste e validação

  
---

**Observação**: Este projeto demonstra a adaptação bem-sucedida de uma estrutura de dados KDTree para um caso de uso específico de reconhecimento facial, mantendo a eficiência e expandindo as funcionalidades.
