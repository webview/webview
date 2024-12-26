# pylint: disable=missing-module-docstring
# pylint: disable=pointless-statement
{
    'targets': [
        {
            'target_name': 'webview.napi',
            'sources': ['webview_napi_wrap.cxx'],
            'include_dirs': [
                "<!@(node -p \"require('node-addon-api').include\")",
                "../../core/include/webview"
            ],
            'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
            "cflags": ["-O2", "-fpermissive"],
            'msvs_settings': {
                'VCCLCompilerTool': {'ExceptionHandling': 1},
            },
            'conditions': [
                ['OS=="mac"',
                 {
                     'xcode_settings': {
                         'GCC_ENABLE_CPP_RTTI': 'YES',
                         'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                         'CLANG_CXX_LIBRARY': 'libc++',
                         'MACOSX_DEPLOYMENT_TARGET': '10.7'
                     },
                     "cflags": ["-std=c++11"]
                 }
                 ],
                ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"',
                 {
                     'include_dirs': [
                         " <!(pkg-config --cflags gtk4 webkitgtk-6.0)",
                     ],
                     "libraries": [" <!(pkg-config --libs gtk4 webkitgtk-6.0)", "-ldl"],
                     "cflags": ["-std=c++11"],
                     'cflags!': ['-fno-exceptions'],
                     'cflags_cc!': ['-fno-exceptions', '-fno-rtti']
                 }
                 ]
            ]
        }

    ]
}
