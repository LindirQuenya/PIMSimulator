#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "pimapi.h"
#include "object.h"


char pimsim_docs[] = "Python module for interfacing with the AquaboltXL Simulator.";
char pimsim_name[] = "pimsim";
char pimsim_init_docs[] = "Reinitializes the simulator state.";
char pimsim_readregister_docs[] = "Reads a register, specified by an integer 0-32. 0-31 is a GRF, 32 is PC.";
char pimsim_writeregister_docs[] = "Writes the second argument to a register, specified by the first argument (same encoding as read).";
char pimsim_readbyte_docs[] = "Reads a byte of memory, specified by its address.";
char pimsim_writebyte_docs[] = "Writes the second argument (a byte) to the specified address (first argument).";

PyMethodDef pimsim_funcs[] = {
	{	"init",
		(PyCFunction)pimsim_init,
		METH_NOARGS,
		pimsim_init_docs},
	{	"read_register",
		(PyCFunction)pimsim_readregister,
		METH_VARARGS,
		pimsim_readregister_docs},
	{	"write_register",
		(PyCFunction)pimsim_writeregister,
		METH_VARARGS,
		pimsim_writeregister_docs},
	{	"read_byte",
		(PyCFunction)pimsim_readbyte,
		METH_VARARGS,
		pimsim_readbyte_docs},
	{	"write_byte",
		(PyCFunction)pimsim_writebyte,
		METH_VARARGS,
		pimsim_writebyte_docs},
	{	"single_step",
		(PyCFunction)pimsim_singlestep,
		METH_NOARGS,
		""},
	{	"execute",
		(PyCFunction)pimsim_execute,
		METH_NOARGS,
		""},
	{	"set_breakpoint",
		(PyCFunction)pimsim_setbreakpoint,
		METH_VARARGS,
		""},
	{	"delete_breakpoint",
		(PyCFunction)pimsim_clearbreakpoint,
		METH_VARARGS,
		""},

	{	NULL}
};

PyModuleDef pimsim_mod = {
	PyModuleDef_HEAD_INIT,
	pimsim_name,
	pimsim_docs,
	-1,
	pimsim_funcs,
	NULL,
	NULL,
	NULL,
	NULL
};

PyMODINIT_FUNC PyInit_pimsim(void) {
	pimsim_init(NULL);
	Py_DECREF(Py_None);
	return PyModule_Create(&pimsim_mod);
}


PyObject *
pimsim_init(PyObject *self)
{
	// TODO make this configurable as an argument to this function.
	pim_init("pim-program.json");

	Py_INCREF(Py_None);
	Py_RETURN_NONE;
}

PyObject *
pimsim_readregister(PyObject *self, PyObject *args)
{
	// Single argument: register number (reg_num).
	// 0-15:  GRF_A[reg_num]
	// 16-31: GRF_B[reg_num - 16]
	// 32: PC I think?
	unsigned int reg_num;
	if (!PyArg_ParseTuple(args, "I", &reg_num)) {
		return NULL;
	}
	
	return PyLong_FromLong(readregister(reg_num));
}

PyObject *
pimsim_writeregister(PyObject *self, PyObject *args)
{
	// Two arguments: register number (reg_num) and value.
	// 0-15:  GRF_A[reg_num]
	// 16-31: GRF_B[reg_num - 16]
	// 32: PC I think?
	unsigned int reg_num;
	// Because it could be more than two bytes if it is PC, we will initially read it into
	// an unsigned long.
	unsigned long input_value;
	// I'm fine with overflow here. We just ignore the bytes that overflow.
	if (!PyArg_ParseTuple(args, "Ik", &reg_num, &input_value)) {
		return NULL;
	}

	writeregister(reg_num, input_value);
	
	Py_INCREF(Py_None);
	Py_RETURN_NONE;
}

PyObject *
pimsim_readbyte(PyObject *self, PyObject *args)
{
	// One argument: address.
	unsigned long addr;
	// I'm fine with overflow here. We just ignore the bytes that overflow.
	if (!PyArg_ParseTuple(args, "k", &addr)) {
		return NULL;
	}
	
	return PyLong_FromLong(readbyte(addr));
}

PyObject *
pimsim_writebyte(PyObject *self, PyObject *args)
{
	// Two arguments: address and value.
	unsigned long addr;
	unsigned char byte;
	// I'm fine with overflow here. We just ignore the bytes that overflow.
	if (!PyArg_ParseTuple(args, "kB", &addr, &byte)) {
		return NULL;
	}

	writebyte(addr, byte);

	Py_INCREF(Py_None);
	Py_RETURN_NONE;
}

PyObject *
pimsim_setbreakpoint(PyObject *self, PyObject *args)
{
	// One argument: location.
	unsigned long addr;
	if (!PyArg_ParseTuple(args, "k", &addr)) {
		return NULL;
	}

	setbreakpoint(addr);

	Py_INCREF(Py_None);
	Py_RETURN_NONE;
}

PyObject *
pimsim_clearbreakpoint(PyObject *self, PyObject *args)
{
	// One argument: location.
	unsigned long addr;
	if (!PyArg_ParseTuple(args, "k", &addr)) {
		return NULL;
	}

	clearbreakpoint(addr);

	Py_INCREF(Py_None);
	Py_RETURN_NONE;
}

PyObject *
pimsim_execute(PyObject *self)
{
	executeprogram();

	Py_INCREF(Py_None);
	Py_RETURN_NONE;
}

PyObject *
pimsim_singlestep(PyObject *self)
{
	singlestep();

	Py_INCREF(Py_None);
	Py_RETURN_NONE;
}