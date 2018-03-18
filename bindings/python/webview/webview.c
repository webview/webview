#include <Python.h>

#include "structmember.h"

#define WEBVIEW_IMPLEMENTATION
#include "webview.h"

typedef struct { PyObject_HEAD struct webview w; } WebView;

static void WebView_dealloc(WebView *self) {
  webview_exit(&self->w);
  Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *WebView_new(PyTypeObject *type, PyObject *args,
                             PyObject *kwds) {
  WebView *self = (WebView *)type->tp_alloc(type, 0);
  if (self == NULL) {
    return NULL;
  }
  memset(&self->w, 0, sizeof(self->w));
  return (PyObject *)self;
}

static int WebView_init(WebView *self, PyObject *args, PyObject *kwds) {
  const char *url = NULL;
  const char *title = NULL;
  static char *kwlist[] = {"width", "height", "resizable", "debug",
                           "url",   "title",  NULL};

  if (!PyArg_ParseTupleAndKeywords(
          args, kwds, "ii|iiss", kwlist, &self->w.width, &self->w.height,
          &self->w.resizable, &self->w.debug, &url, &title)) {
    return -1;
  }

  self->w.url = url;
  self->w.title = title;

  return webview_init(&self->w);
}

static PyObject *WebView_run(WebView *self) {
  while (webview_loop(&self->w, 1) == 0)
    ;
  Py_RETURN_NONE;
}

static PyObject *WebView_loop(WebView *self, PyObject *args, PyObject *kwds) {
  int blocking = 1;
  static char *kwlist[] = {"blocking", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist, &blocking)) {
    return NULL;
  }
  if (webview_loop(&self->w, blocking) == 0) {
    Py_RETURN_TRUE;
  } else {
    Py_RETURN_FALSE;
  }
}

static PyObject *WebView_terminate(WebView *self) {
  webview_terminate(&self->w);
  Py_RETURN_NONE;
}

static PyObject *WebView_set_title(WebView *self, PyObject *args) {
  const char *title = "";
  if (!PyArg_ParseTuple(args, "s", &title)) {
    return NULL;
  }
  webview_set_title(&self->w, title);
  Py_RETURN_NONE;
}

static PyObject *WebView_set_fullscreen(WebView *self, PyObject *args) {
  int fullscreen = 0;
  if (!PyArg_ParseTuple(args, "i", &fullscreen)) {
    return NULL;
  }
  webview_set_fullscreen(&self->w, fullscreen);
  Py_RETURN_NONE;
}

static PyObject *WebView_set_color(WebView *self, PyObject *args) {
  int r, g, b, a = 255;
  if (!PyArg_ParseTuple(args, "iii|i", &r, &g, &b, &a)) {
    return NULL;
  }
  webview_set_color(&self->w, r, g, b, a);
  Py_RETURN_NONE;
}

static PyObject *WebView_eval(WebView *self, PyObject *args) {
  const char *js = NULL;
  if (!PyArg_ParseTuple(args, "s", &js)) {
    return NULL;
  }
  webview_eval(&self->w, js);
  Py_RETURN_NONE;
}

static PyObject *WebView_inject_css(WebView *self, PyObject *args) {
  const char *css = NULL;
  if (!PyArg_ParseTuple(args, "s", &css)) {
    return NULL;
  }
  webview_inject_css(&self->w, css);
  Py_RETURN_NONE;
}

static PyObject *WebView_dialog(WebView *self, PyObject *args, PyObject *kwds) {
  int type = 0;
  int flags = 0;
  const char *title = NULL;
  const char *arg = NULL;
  char result[PATH_MAX];
  static char *kwlist[] = {"type", "flags", "title", "arg", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "iiss", kwlist, &type, &flags,
                                   &title, &arg)) {
    return NULL;
  }
  webview_dialog(&self->w, type, flags, title, arg, result, sizeof(result));
  return PyUnicode_FromString(result);
}

static void webview_dispatch_cb(struct webview *w, void *arg) {
  PyObject *cb = (PyObject *)arg;
  /* TODO */
  PyObject_CallObject(cb, NULL);
  Py_XINCREF(cb);
}

static PyObject *WebView_dispatch(WebView *self, PyObject *args) {
  PyObject *tmp;
  if (!PyArg_ParseTuple(args, "O:set_callback", &tmp)) {
    return NULL;
  }
  if (!PyCallable_Check(tmp)) {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable");
    return NULL;
  }
  Py_XINCREF(tmp);
  webview_dispatch(&self->w, webview_dispatch_cb, tmp);
  Py_RETURN_NONE;
}

static PyObject *WebView_bind(WebView *self) {
  /* TODO, very complex implementation */
  Py_RETURN_NONE;
}

static PyMemberDef WebView_members[] = {
    {NULL} /* Sentinel */
};
static PyMethodDef WebView_methods[] = {
    {"run", (PyCFunction)WebView_run, METH_NOARGS, "..."},
    {"loop", (PyCFunction)WebView_loop, METH_KEYWORDS | METH_VARARGS, "..."},
    {"terminate", (PyCFunction)WebView_terminate, METH_NOARGS, "..."},
    {"dispatch", (PyCFunction)WebView_dispatch, METH_VARARGS, "..."},
    {"eval", (PyCFunction)WebView_eval, METH_VARARGS, "..."},
    {"inject_css", (PyCFunction)WebView_inject_css, METH_VARARGS, "..."},
    {"dialog", (PyCFunction)WebView_dialog, METH_KEYWORDS | METH_VARARGS, "..."},
    {"set_title", (PyCFunction)WebView_set_title, METH_VARARGS, "..."},
    {"set_fullscreen", (PyCFunction)WebView_set_fullscreen, METH_VARARGS,
     "..."},
    {"set_color", (PyCFunction)WebView_set_color, METH_VARARGS, "..."},
    {"bind", (PyCFunction)WebView_bind, METH_VARARGS, "..."},
    {NULL} /* Sentinel */
};

static PyTypeObject WebViewType = {
    PyVarObject_HEAD_INIT(NULL, 0) "webview.WebView", /* tp_name */
    sizeof(WebView),                                  /* tp_basicsize */
    0,                                                /* tp_itemsize */
    (destructor)WebView_dealloc,                      /* tp_dealloc */
    0,                                                /* tp_print */
    0,                                                /* tp_getattr */
    0,                                                /* tp_setattr */
    0,                                                /* tp_compare */
    0,                                                /* tp_repr */
    0,                                                /* tp_as_number */
    0,                                                /* tp_as_sequence */
    0,                                                /* tp_as_mapping */
    0,                                                /* tp_hash */
    0,                                                /* tp_call */
    0,                                                /* tp_str */
    0,                                                /* tp_getattro */
    0,                                                /* tp_setattro */
    0,                                                /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,         /* tp_flags */
    "WebView objects",                                /* tp_doc */
    0,                                                /* tp_traverse */
    0,                                                /* tp_clear */
    0,                                                /* tp_richcompare */
    0,                                                /* tp_weaklistoffset */
    0,                                                /* tp_iter */
    0,                                                /* tp_iternext */
    WebView_methods,                                  /* tp_methods */
    WebView_members,                                  /* tp_members */
    0,                                                /* tp_getset */
    0,                                                /* tp_base */
    0,                                                /* tp_dict */
    0,                                                /* tp_descr_get */
    0,                                                /* tp_descr_set */
    0,                                                /* tp_dictoffset */
    (initproc)WebView_init,                           /* tp_init */
    0,                                                /* tp_alloc */
    WebView_new,                                      /* tp_new */
};

static PyMethodDef module_methods[] = {
    {NULL} /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "webview",
        "Example module",
        0,
        module_methods,
        NULL,
        NULL,
        NULL,
        NULL
};
#define MODINIT_ERROR NULL
#define MODINIT_NAME PyInit_webview
#else
#define MODINIT_ERROR
#define MODINIT_NAME initwebview
#endif
PyMODINIT_FUNC MODINIT_NAME(void) {
  PyObject *m;

  if (PyType_Ready(&WebViewType) < 0) {
    return MODINIT_ERROR;
  }

#if PY_MAJOR_VERSION >= 3
    m = PyModule_Create(&moduledef);
#else
    m = Py_InitModule3("webview", module_methods,
        "Example module that creates an extension type.");
#endif
  if (m == NULL) {
    return MODINIT_ERROR;
  }

  Py_INCREF(&WebViewType);
  PyModule_AddObject(m, "WebView", (PyObject *)&WebViewType);
#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}
