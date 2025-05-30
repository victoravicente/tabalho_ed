# API de Busca por Similaridade usando KD-Tree

Este projeto implementa uma API para busca por similaridade usando uma estrutura de dados KD-Tree. A implementação permite armazenar e buscar embeddings (vetores de 128 dimensões) de forma eficiente.

## Estrutura do Projeto

O projeto está organizado da seguinte forma:
- `api/kdtree.c`: Implementação da estrutura KD-Tree em C
- `api/kdtree_wrapper.py`: Wrapper Python para interface com o código C
- `api/app.py`: API FastAPI para endpoints de serviço

## Funcionalidades

- Construção dinâmica de KD-Tree
- Inserção de embeddings com identificadores
- Busca do vizinho mais próximo
- Busca dos N vizinhos mais próximos
- Interface REST API para todas as operações

## Endpoints da API

### POST /construir-arvore
Inicializa uma nova árvore KD.

### POST /inserir
Insere um novo embedding na árvore.
```json
{
    "embedding": [float * 128],
    "id": "string"
}
```

### GET /buscar_n
Busca os N vizinhos mais próximos de um embedding.
- Parâmetros:
  - `embedding`: Lista com 128 valores float
  - `n`: Número de vizinhos (default: 5)

## Tecnologias Utilizadas

- C (implementação core da KD-Tree)
- Python 3
- FastAPI
- ctypes (integração C/Python)

## Como Executar

1. Compile a biblioteca C:
```bash
gcc -shared -o libkdtree.so -fPIC api/kdtree.c
```

2. Instale as dependências Python:
```bash
pip install fastapi uvicorn
```

3. Execute a API:
```bash
uvicorn api.app:app --reload
```

## Repositório

O código fonte está disponível em: [https://github.com/victoravicente/tabalho_ed](https://github.com/victoravicente/tabalho_ed)

## Licença

MIT

