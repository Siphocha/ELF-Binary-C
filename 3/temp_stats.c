#include <Python.h>
#include <math.h>

/* Struct to hold temp and stats */
typedef struct {
    double *readings;     
    size_t count;         
    double min;           
    double max;           
    double sum;           
    double mean;          
    double variance;      
    int is_calculated;    
} TempData;

/*
 O(1) time algo for calculating stats in real time.
 */
static void calculate_stats(TempData *data) {
    if (data->count == 0 || data->is_calculated) {
        return;
    }
    
    data->min = data->readings[0];
    data->max = data->readings[0];
    data->sum = 0.0;
    
    for (size_t i = 0; i < data->count; i++) {
        double temp = data->readings[i];
        
        if (temp < data->min) data->min = temp;
        if (temp > data->max) data->max = temp;
        data->sum += temp;
    }
    
    //Calculating mean
    data->mean = data->sum / data->count;
    
    //Calculating variance
    double sum_sq_diff = 0.0;
    for (size_t i = 0; i < data->count; i++) {
        double diff = data->readings[i] - data->mean;
        sum_sq_diff += diff * diff;
    }
    
    //Variance last denominator
    data->variance = (data->count > 1) ? sum_sq_diff / (data->count - 1) : 0.0;
    data->is_calculated = 1;
}

/*
 * 1. init_temp_data
 * 2. Initialize TempData structure with readings
 * 3. Stores reference to input array (no copy for efficiency)
 */
static TempData* init_temp_data(PyObject *readings_obj) {
    if (!PyList_Check(readings_obj)) {
        PyErr_SetString(PyExc_TypeError, "Expecting floats");
        return NULL;
    }
    
    TempData *data = (TempData*)malloc(sizeof(TempData));
    if (!data) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory");
        return NULL;
    }
    
    //Getting list size
    data->count = PyList_Size(readings_obj);
    
    if (data->count == 0) {
        free(data);
        PyErr_SetString(PyExc_ValueError, "Empty temperature list");
        return NULL;
    }
    
    //Allocate array for readings
    data->readings = (double*)malloc(data->count * sizeof(double));
    if (!data->readings) {
        free(data);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for readings");
        return NULL;
    }
    
    for (size_t i = 0; i < data->count; i++) {
        PyObject *item = PyList_GetItem(readings_obj, i);
        if (!PyFloat_Check(item)) {
            free(data->readings);
            free(data);
            PyErr_SetString(PyExc_TypeError, "All list items must be floats");
            return NULL;
        }
        data->readings[i] = PyFloat_AsDouble(item);
    }
    
    data->is_calculated = 0;
    return data;
}

/*
 * 1. free_temp_data
 * 2. Clean up allocated memory for TempData
 * 3. Free's both the readings array and the structure
 */
static void free_temp_data(TempData *data) {
    if (data) {
        if (data->readings) {
            free(data->readings);
        }
        free(data);
    }
}

/*
 * 1. py_min_temp
 * 2. Return minimum temperature from readings
 * 3. O(n) - calls calculate_stats
 */
static PyObject* py_min_temp(PyObject *self, PyObject *args) {
    PyObject *readings_obj;
    
    if (!PyArg_ParseTuple(args, "O", &readings_obj)) {
        return NULL;
    }
    
    TempData *data = init_temp_data(readings_obj);
    if (!data) {
        return NULL;
    }
    
    calculate_stats(data);
    double result = data->min;
    free_temp_data(data);
    
    return PyFloat_FromDouble(result);
}

/*
 * 1. py_max_temp
 * 2. Return maximum temperature from readings of data
 * 3. O(n) - calls calculate_stats 
 */
static PyObject* py_max_temp(PyObject *self, PyObject *args) {
    PyObject *readings_obj;
    
    if (!PyArg_ParseTuple(args, "O", &readings_obj)) {
        return NULL;
    }
    
    TempData *data = init_temp_data(readings_obj);
    if (!data) {
        return NULL;
    }
    
    calculate_stats(data);
    double result = data->max;
    free_temp_data(data);
    
    return PyFloat_FromDouble(result);
}

/*
 * Function: py_avg_temp
 * Purpose: Return average (mean) temperature from readings
 * Time Complexity: O(n) - calls calculate_stats which is O(n)
 */
static PyObject* py_avg_temp(PyObject *self, PyObject *args) {
    PyObject *readings_obj;
    
    if (!PyArg_ParseTuple(args, "O", &readings_obj)) {
        return NULL;
    }
    
    TempData *data = init_temp_data(readings_obj);
    if (!data) {
        return NULL;
    }
    
    calculate_stats(data);
    double result = data->mean;
    free_temp_data(data);
    
    return PyFloat_FromDouble(result);
}

/*
 * 1. py_variance_temp
 * 2. Return sample variance of temperature readings
 * 3. O(n) - calls calculate_stats
 */
static PyObject* py_variance_temp(PyObject *self, PyObject *args) {
    PyObject *readings_obj;
    
    if (!PyArg_ParseTuple(args, "O", &readings_obj)) {
        return NULL;
    }
    
    TempData *data = init_temp_data(readings_obj);
    if (!data) {
        return NULL;
    }
    
    calculate_stats(data);
    double result = data->variance;
    free_temp_data(data);
    
    return PyFloat_FromDouble(result);
}

/*
 * 1. py_count_readings
 * 2. Return total number of temperature readings
 * 3. O(1) - direct access to count
 */
static PyObject* py_count_readings(PyObject *self, PyObject *args) {
    PyObject *readings_obj;
    
    if (!PyArg_ParseTuple(args, "O", &readings_obj)) {
        return NULL;
    }
    
    if (!PyList_Check(readings_obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list, but didn't get it. ");
        return NULL;
    }
    
    Py_ssize_t count = PyList_Size(readings_obj);
    return PyLong_FromSsize_t(count);
}

/* Module method table */
static PyMethodDef TempStatsMethods[] = {
    {"min_temp", py_min_temp, METH_VARARGS, "Returns the min temp from the readings list"},
    {"max_temp", py_max_temp, METH_VARARGS, "Returns the max temp from the readings list"},
    {"avg_temp", py_avg_temp, METH_VARARGS, "Returns the avg temp from the readings list"},
    {"variance_temp", py_variance_temp, METH_VARARGS, "Returns sample variance of temp"},
    {"count_readings", py_count_readings, METH_VARARGS, "Returns total num of temp readings"},
    {NULL, NULL, 0, NULL} 
};

/* Module definition */
static struct PyModuleDef tempstatsmodule = {
    PyModuleDef_HEAD_INIT,
    "tempstats",           
    "C extension! ", 
    -1,                   
    TempStatsMethods
};

/* Module initialization function */
PyMODINIT_FUNC PyInit_tempstats(void) {
    return PyModule_Create(&tempstatsmodule);
}