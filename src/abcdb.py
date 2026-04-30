import json
import numpy as np

import bitbarrel 

class abcDB:
    def __init__(self, db_path="vector_data"):
        self.db = bitbarrel.BitBarrel(db_path)
        self.index_key = "__vector_keys__"
        self._keys = self._load_keys()

    def _load_keys(self):
        try:
            data_bytes = self.db.get(self.index_key)
            return json.loads(data_bytes.decode('utf-8'))
        except KeyError:
            return []

    def _save_keys(self):
        data_bytes = json.dumps(self._keys).encode('utf-8')
        self.db.set(self.index_key, data_bytes)

    def add(self, key: str, vector, metadata: dict = None):
        if not isinstance(vector, np.ndarray):
            vector = np.array(vector)
            
        payload = {
            "vector": vector.tolist(),
            "metadata": metadata or {}
        }
        
        payload_bytes = json.dumps(payload).encode('utf-8')
        
        self.db.set(key, payload_bytes)
        
        if key not in self._keys:
            self._keys.append(key)
            self._save_keys()

    def get(self, key: str):
        try:
            data_bytes = self.db.get(key)
            payload = json.loads(data_bytes.decode('utf-8'))
            return {
                "vector": np.array(payload["vector"]),
                "metadata": payload["metadata"]
            }
        except KeyError:
            return None

    def search(self, query_vector, k=5):
        if not isinstance(query_vector, np.ndarray):
            query_vector = np.array(query_vector)

        results = []

        for key in self._keys:
            try:
                data_bytes = self.db.get(key)
                payload = json.loads(data_bytes.decode('utf-8'))
                stored_vector = np.array(payload["vector"])
                
                distance = np.linalg.norm(stored_vector - query_vector)
                
                results.append({
                    "key": key,
                    "distance": float(distance),
                    "metadata": payload["metadata"]
                })
            except KeyError:
                continue

        results.sort(key=lambda x: x["distance"])
        
        return results[:k]
    