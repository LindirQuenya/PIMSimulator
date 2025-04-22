#define PY_SSIZE_T_CLEAN
#include <Python.h>

char pimsim_docs[] = "Python module for interfacing with the AquaboltXL Simulator.";
char pimsim_name[] = "pimsim";

PyModuleDef pimsim_mod = {
	PyModuleDef_HEAD_INIT,
	pimsim_name,
	pimsim_docs,
	-1,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

PyMODINIT_FUNC PyInit_pimsim(void) {
	return PyModule_Create(&pimsim_mod);
}