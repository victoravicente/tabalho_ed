# app.py
from fastapi import FastAPI
from kdtree_wrapper import lib, Tarv, TReg
from ctypes import POINTER, c_char, c_float
from pydantic import BaseModel
from typing import List

app = FastAPI()

class EmbeddingEntrada(BaseModel):
    embedding: List[float]
    id: str

@app.post("/construir-arvore")
def constroi_arvore():
    lib.kdtree_construir()
    return {"mensagem": "Árvore KD inicializada com sucesso."}

@app.post("/inserir-face")
def inserir_face(dados: EmbeddingEntrada):
    if len(dados.embedding) != 128:
        return {"erro": "O embedding deve ter exatamente 128 dimensões"}
    
    emb = (c_float * 128)(*dados.embedding)
    id_bytes = dados.id.encode('utf-8')[:99]  # Limita a 99 caracteres + null terminator
    novo_ponto = TReg(embedding=emb, id=id_bytes)
    lib.inserir_ponto(novo_ponto)
    return {"mensagem": f"Face de '{dados.id}' inserida com sucesso."}

@app.post("/buscar-faces")
async def buscar_faces(embedding: List[float], n: int = 5):
    if len(embedding) != 128:
        return {"erro": "O embedding deve ter exatamente 128 dimensões"}
    
    emb = (c_float * 128)(*embedding)
    query = TReg(embedding=emb, id=b"")
    
    arv = lib.get_tree()
    saida_type = POINTER(TReg) * n
    saida = saida_type()
    
    lib.buscar_n_vizinhos(arv, query, n, saida)
    
    resultados = []
    for i in range(n):
        if saida[i]:
            reg = saida[i].contents
            resultados.append({
                "id": reg.id.decode('utf-8').rstrip('\x00'),
                "similaridade": 1.0 - (reg.distancia if hasattr(reg, 'distancia') else 0)
            })
    
    return resultados

