# app.py
from fastapi import FastAPI, Query
from kdtree_wrapper import lib, Tarv, TReg
from ctypes import POINTER,c_char
from pydantic import BaseModel
from typing import List

app = FastAPI()


class PontoEntrada(BaseModel):
    lat: float
    lon: float
    nome: str

class EmbeddingEntrada(BaseModel):
    embedding: List[float]
    id: str

@app.post("/construir-arvore")
def constroi_arvore():
    lib.kdtree_construir()
    return {"mensagem": "Árvore KD inicializada com sucesso."}

@app.post("/inserir")
def inserir(ponto: EmbeddingEntrada):
    emb = (ctypes.c_float * 128)(*ponto.embedding)
    id_bytes = ponto.id.encode('utf-8')[:99]
    novo_ponto = TReg(embedding=emb, id=id_bytes)
    lib.inserir_ponto(novo_ponto)
    return {"mensagem": f"Ponto '{ponto.id}' inserido com sucesso."}

@app.get("/buscar")
def buscar(lat: float = Query(...), lon: float = Query(...)):
    query = TReg(lat=lat, lon=lon)

    arv = lib.get_tree()  # Suponha que esta função retorne ponteiro para árvore já construída
    resultado = lib.buscar_mais_proximo(arv, query)

    return {
        "lat": resultado.lat,
        "lon": resultado.lon,
        "nome": resultado.nome
    }

@app.get("/buscar_n")
def buscar_n(embedding: List[float], n: int = 5):
    emb = (ctypes.c_float * 128)(*embedding)
    query = TReg(embedding=emb, id=b"")
    arv = lib.get_tree()
    saida_type = ctypes.POINTER(TReg) * n
    saida = saida_type()
    lib.buscar_n_vizinhos(arv, ctypes.byref(query), n, saida)
    resultados = []
    for i in range(n):
        if saida[i]:
            reg = saida[i].contents
            resultados.append({
                "id": reg.id.decode('utf-8').rstrip('\x00'),
                "embedding": list(reg.embedding)
            })
    return resultados

