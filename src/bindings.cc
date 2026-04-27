#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "bitbarrel.h"

namespace py = pybind11;

PYBIND11_MODULE(bitbarrel, m) {
    m.doc() = "Python bindings for the BitBarrel key-value store";

    py::class_<BitBarrel>(m, "BitBarrel")
        .def(py::init<const std::string &>(), py::arg("dir_name"), 
             "Initialize or open a BitBarrel database in the given directory.")
        
        .def("set", [](BitBarrel& self, const std::string& key, const py::bytes& value) {
            std::string val_str = value;
            Status status;
            
            {
                py::gil_scoped_release release;
                status = self.set(key, val_str);
            }

            if (status != Status::Ok) {
                throw std::runtime_error("BitBarrel: Failed to set value. Unrecoverable write error.");
            }
        }, py::arg("key"), py::arg("value"), "Set a key-value pair in the database.")
        
        .def("get", [](BitBarrel& self, const std::string& key) -> py::object {
            bool found = false;
            std::string val_str;

            {
                py::gil_scoped_release release;
                auto result = self.get(key);
                
                if (result.isOk()) {
                    found = true;
                    val_str = result.value.value(); 
                }
            }

            if (found) {
                return py::bytes(val_str);
            }
            
            throw py::key_error(key);
        }, py::arg("key"), "Get a value by key. Raises KeyError if the key does not exist.");
}
