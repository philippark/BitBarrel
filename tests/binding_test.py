import pytest
import threading
import bitbarrel

@pytest.fixture
def db_path(tmp_path):
    """Provides an isolated temporary directory for the database."""
    # tmp_path is a built-in pytest fixture that creates a unique temp dir
    dir_name = str(tmp_path / "test_db")
    return dir_name

def test_type_conversion_and_basic_io(db_path):
    # Test that initialization works with a standard python string path
    db = bitbarrel.BitBarrel(db_path)
    
    # Test that the binding accepts string keys and byte values
    db.set("greeting", b"hello world")
    db.set("binary_data", b"\x00\x01\x02\xFF")
    
    # Test that the returned object is strictly Python bytes
    res = db.get("greeting")
    assert isinstance(res, bytes)
    assert res == b"hello world"
    
    assert db.get("binary_data") == b"\x00\x01\x02\xFF"

def test_exception_translation(db_path):
    db = bitbarrel.BitBarrel(db_path)
    
    # The binding explicitly raises py::key_error. 
    # We must ensure Python sees this as a standard KeyError.
    with pytest.raises(KeyError) as exc_info:
        db.get("missing_key")
    
    # Verify the error message contains the key
    assert "missing_key" in str(exc_info.value)

def test_gil_release_multithreading(db_path):
    db = bitbarrel.BitBarrel(db_path)
    
    def worker_write(thread_id):
        for i in range(100):
            key = f"thread_{thread_id}_key_{i}"
            val = f"val_{i}".encode('utf-8')
            db.set(key, val)

    threads = []
    # Launch multiple Python threads to ensure py::gil_scoped_release 
    # correctly allows concurrent C++ execution
    for i in range(5):
        t = threading.Thread(target=worker_write, args=(i,))
        threads.append(t)
        t.start()

    for t in threads:
        t.join()

    # Verify a sample to ensure threads didn't block and crash
    assert db.get("thread_0_key_50") == b"val_50"
    assert db.get("thread_4_key_99") == b"val_99"
