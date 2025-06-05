import requests
import numpy as np
import pandas as pd

BASE_URL = "http://127.0.0.1:8000"

def carregar_embeddings():
    """Carrega os embeddings do arquivo parquet"""
    df = pd.read_parquet("embeddings.parquet")
    # Converte os embeddings numpy para lista Python
    embeddings = [emb.tolist() if isinstance(emb, np.ndarray) else emb for emb in df["emb"]]
    return df["person_name"].tolist(), embeddings

def test_construir_arvore():
    """Testa a construção da árvore KD"""
    response = requests.post(f"{BASE_URL}/construir-arvore")
    print("Resposta da construção da árvore:", response.json())

def test_inserir_face(embedding, person_name):
    """Testa a inserção de uma face"""
    # Garante que o embedding está no formato de lista
    if isinstance(embedding, np.ndarray):
        embedding = embedding.tolist()
    
    payload = {
        "embedding": embedding,
        "id": person_name
    }
    
    response = requests.post(
        f"{BASE_URL}/inserir-face",
        json=payload
    )
    print(f"Resposta da inserção para {person_name}:", response.json())

def test_buscar_faces(embedding_consulta):
    """Testa a busca de faces similares"""
    response = requests.post(
        f"{BASE_URL}/buscar-faces",
        json=embedding_consulta,
        params={"n": 5}
    )
    print("Resposta da busca:", response.json())

def executar_todos_testes():
    print("=== Iniciando testes da API ===\n")
    
    # Carrega os dados do arquivo
    nomes, embeddings = carregar_embeddings()
    
    # 1. Primeiro constrói a árvore
    print("1. Testando construção da árvore...")
    test_construir_arvore()
    
    # 2. Insere algumas faces do arquivo
    print("\n2. Testando inserção de faces...")
    for i, (nome, emb) in enumerate(zip(nomes, embeddings)):
        print(f"\nInserindo face {i+1} - {nome}...")
        test_inserir_face(emb, nome)
        if i >= 1000:  # Limita a 10 inserções para teste
            break
    
    # 3. Busca faces similares usando um embedding real
    print("\n3. Testando busca de faces...")
    test_buscar_faces(embeddings[0])  # Usa o primeiro embedding como consulta

if __name__ == "__main__":
    executar_todos_testes()
