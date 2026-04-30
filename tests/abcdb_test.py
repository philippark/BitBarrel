import pytest
import numpy as np
from abcdb import abcDB

@pytest.fixture
def temp_db_path(tmp_path):
    """Provides an isolated temporary directory for the database."""
    return str(tmp_path / "vector_test_db")

def test_add_and_get_type_handling(temp_db_path):
    db = abcDB(temp_db_path)
    
    # Test adding a standard Python list
    list_vec = [1.0, 2.0, 3.0]
    db.add("list_vec", list_vec, {"type": "list"})
    
    # Test adding a NumPy array
    np_vec = np.array([4.0, 5.0, 6.0])
    db.add("np_vec", np_vec, {"type": "numpy"})

    # Verify retrieval
    res_list = db.get("list_vec")
    assert res_list is not None
    assert isinstance(res_list["vector"], np.ndarray)  # Should always return ndarray
    assert np.array_equal(res_list["vector"], np.array(list_vec))
    assert res_list["metadata"] == {"type": "list"}

    res_np = db.get("np_vec")
    assert np.array_equal(res_np["vector"], np_vec)

def test_vector_search_logic(temp_db_path):
    db = abcDB(temp_db_path)
    
    # Add vectors with known Euclidean distances
    db.add("origin", [0.0, 0.0], {"desc": "center"})
    db.add("y_axis", [0.0, 2.0], {"desc": "up"})
    db.add("far_away", [10.0, 10.0], {"desc": "distant"})
    
    # Query near the origin
    query = [0.0, 0.5]
    results = db.search(query, k=2)
    
    # We requested k=2, so we should get the 2 closest vectors
    assert len(results) == 2
    
    # Closest should be "origin" (distance 0.5)
    assert results[0]["key"] == "origin"
    assert results[0]["distance"] == 0.5
    
    # Second closest should be "y_axis" (distance 1.5)
    assert results[1]["key"] == "y_axis"
    assert results[1]["distance"] == 1.5

def test_persistence_and_index_reloading(temp_db_path):
    # Initialize db and add data
    db1 = abcDB(temp_db_path)
    db1.add("persist_1", [1.1, 2.2])
    db1.add("persist_2", [3.3, 4.4], {"saved": True})
    
    # Re-initialize the database from the same path
    db2 = abcDB(temp_db_path)
    
    # Check if the keys index was properly loaded
    assert "persist_1" in db2._keys
    assert "persist_2" in db2._keys
    
    # Check if the data is intact
    res = db2.get("persist_2")
    assert res is not None
    assert res["metadata"] == {"saved": True}
    assert np.array_equal(res["vector"], np.array([3.3, 4.4]))

def test_missing_keys_and_overwrites(temp_db_path):
    db = abcDB(temp_db_path)
    
    # Test getting a non-existent key
    assert db.get("ghost_key") is None
    
    # Test overwriting an existing key
    db.add("target", [1.0, 1.0], {"version": 1})
    db.add("target", [9.0, 9.0], {"version": 2})
    
    res = db.get("target")
    
    # Ensure the data was updated
    assert np.array_equal(res["vector"], np.array([9.0, 9.0]))
    assert res["metadata"] == {"version": 2}
    
    # Ensure the key index didn't duplicate the key
    assert db._keys.count("target") == 1
