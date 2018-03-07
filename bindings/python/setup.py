#!/bin/env python

import os
import commands
import shutil

from distutils.core import setup
from distutils.extension import Extension
from distutils.cmd import Command


class sync_command(Command):
    """A custom command to synchronize top-level webview.h implementation
    with the one included in the Python bindings source package."""
    description = 'synchronize webview.h'
    user_options = []

    def initialize_options(self):
        pass

    def finalize_options(self):
        pass

    def run(self):
        shutil.copyfile('../../webview.h', 'webview/webview.h')
        pass


if hasattr(os, 'uname'):
    OSNAME = os.uname()[0]
else:
    OSNAME = 'Windows'

if OSNAME == 'Linux':

    def pkgconfig(flags):
        return commands.getoutput(
            "pkg-config %s gtk+-3.0 webkit2gtk-4.0" % flags)

    define_macros = [("WEBVIEW_GTK", '1')]
    extra_cflags = pkgconfig("--cflags").split()
    extra_ldflags = pkgconfig("--libs").split()
elif OSNAME == 'Darwin':
    define_macros = [('WEBVIEW_COCOA', '1')]
    extra_cflags = ""
    extra_ldflags = ['-framework', 'CoreAudio']
elif OSNAME == 'Windows':
    define_macros = [('WEBVIEW_WINAPI', '1')]
    extra_cflags = ""
    extra_ldflags = ['-framework', 'CoreAudio']

webview = Extension(
    'webview',
    sources=['webview/webview.c'],
    define_macros=define_macros,
    extra_compile_args=extra_cflags,
    extra_link_args=extra_ldflags,
)

setup(
    name='webview',
    version='0.1.4',
    description='Python WebView bindings',
    author='Serge Zaitsev',
    author_email='zaitsev.serge@gmail.com',
    url='https://github.com/zserge/webview',
    keywords=[],
    license='MIT',
    classifiers=[],
    ext_modules=[webview],
    cmdclass={'sync': sync_command},
)
