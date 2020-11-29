/*
	PyWebUI Library
	- - - - - - -
	http://webui.me
	https://github.com/alifcommunity/webui
	Licensed under GNU General Public License v3.0.
	Copyright (C)2020 Hassan DRAGA <https://github.com/hassandraga>.
*/

#ifndef UNICODE
#define UNICODE
#endif

// C++ headers
#include <iostream>
#include <string>
#include <new>
#include <thread>
#include <vector>

// WebUI headers
#include <webui/webui.hpp>

// Python headers
#define PY_SSIZE_T_CLEAN
#include <Python.h>

// Defines
#define DLLEXPORT extern "C" __declspec(dllexport)
bool py_initialized = false;
static std::vector<PyObject *> callback_obj_v;
static std::vector<void (*)()> callback_void_v;

DLLEXPORT void py_handler(webui::event e) {

    if (!PyCallable_Check(callback_obj_v[e.id])){

        std::cerr << "WebUI Err: callback obj not callable. " << std::endl;
        return;
    }

    try {

        // Calling python function using PyObject
        // PyObject *result;
        // result = PyObject_CallObject(callback_obj_v[e.id], NULL); <-- This crash python interepter!
        // if(result == NULL)
        //     std::cerr << "WebUI Err: failed to call obj (null return). " << std::endl;
        
        // Calling python function using void pointer
        if(callback_void_v[e.id])
            (*callback_void_v[e.id])();
    }
    catch(...) {

        std::cerr << "WebUI Err: failed to call obj (exception). " << std::endl;
    }
}

DLLEXPORT void * py_create_window(void) {

    return new(std::nothrow) webui::window;
}

DLLEXPORT void py_destroy_window(void *ptr) {

    if(ptr)
        delete ptr;
}

DLLEXPORT int py_show_window(void *ptr, char * html) {

    try {

        std::string _html = html;
        webui::window * ref = reinterpret_cast<webui::window *>(ptr);
        return ref->show(_html);
    }
    catch(...) {

        return -1;
    }
}

DLLEXPORT void py_bind_element(void *ptr, char const *id, PyObject *callback_obj, void (*callback_void)()) {

    // callback_obj:    python function as PyObject
    // callback_void:   python function as void pointer

    static PyObject *callback_obj_temp = NULL;
    std::string _id = id;
    webui::window * ref = reinterpret_cast<webui::window *>(ptr);

    if (!PyCallable_Check(callback_obj)){

        std::cerr << "WebUI Err: callback obj not callable. " << std::endl;
        return;
    }

    int elem_id = ref->bind(_id, py_handler);

    Py_XINCREF(callback_obj);           // Add a reference to new callback
    Py_XDECREF(callback_obj_temp);      // Dispose of previous callback
    callback_obj_temp = callback_obj;   // Remember new callback

    callback_obj_v.push_back(callback_obj_temp);
    callback_void_v.push_back(callback_void);
}

DLLEXPORT void py_loop (void) {

    std::thread ui(webui::loop);
    ui.join();
}

DLLEXPORT bool py_any_is_show (void) {

    return webui::any_is_show();
}

DLLEXPORT void py_ini (void) {

    if(!py_initialized){

        py_initialized = true;
        callback_obj_v.clear();
        callback_obj_v.push_back(NULL);
        callback_void_v.clear();
        callback_void_v.push_back(NULL);
        webui::ini();
    }
}
