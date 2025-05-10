#ifndef DEBUGGER_PIMSIM_PIMAPI_H
#define DEBUGGER_PIMSIM_PIMAPI_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string>

PyObject *pimsim_init(PyObject *self);
PyObject *pimsim_readbyte(PyObject *self, PyObject *args);
PyObject *pimsim_writebyte(PyObject *self, PyObject *args);
PyObject *pimsim_readregister(PyObject *self, PyObject *args);
PyObject *pimsim_writeregister(PyObject *self, PyObject *args);
PyObject *pimsim_setbreakpoint(PyObject *self, PyObject *args);
PyObject *pimsim_clearbreakpoint(PyObject *self, PyObject *args);
PyObject *pimsim_execute(PyObject *self);
PyObject *pimsim_singlestep(PyObject *self);

void pim_init(std::string fname);
uint64_t readregister(unsigned int reg_num);
void writeregister(unsigned int reg_num, unsigned long input_value);
unsigned char readbyte(uint64_t addr);
void writebyte(uint64_t addr, unsigned char byte);
void executeprogram(void);
void setbreakpoint(size_t where);
void clearbreakpoint(size_t where);
void singlestep(void);

#endif