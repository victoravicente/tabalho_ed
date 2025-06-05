import ctypes
from ctypes import Structure, POINTER, c_float, c_char

class TReg(Structure):
    _fields_ = [("embedding", c_float * 128),
                ("id", c_char * 100)]

class TNode(Structure):
    pass

TNode._fields_ = [("key", ctypes.c_void_p),
                   ("esq", POINTER(TNode)),
                   ("dir", POINTER(TNode))]

class Tarv(Structure):
    _fields_ = [("k", ctypes.c_int),
                ("dist", ctypes.c_void_p),
                ("cmp", ctypes.c_void_p),
                ("raiz", POINTER(TNode))]

lib = ctypes.CDLL("/mnt/v/facom/ed/trabalho/tabalho_ed/api/libkdtree.so")

# Função para buscar N vizinhos mais próximos
lib.buscar_n_vizinhos.argtypes = [POINTER(Tarv), POINTER(TReg), ctypes.c_int, ctypes.POINTER(ctypes.POINTER(TReg))]
lib.buscar_n_vizinhos.restype = None

lib.get_tree.restype = POINTER(Tarv)
lib.inserir_ponto.argtypes = [TReg]
lib.inserir_ponto.restype = None
lib.kdtree_construir.argtypes = []
lib.kdtree_construir.restype = None
