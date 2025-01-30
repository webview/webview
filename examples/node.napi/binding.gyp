# pylint: disable=missing-module-docstring
# pylint: disable=pointless-statement
{
    'targets': [
        {
            'target_name': 'webview.napi',
            'sources': ['src/webview.napi_wrap.cxx'],
            'include_dirs': [
                "<!@(node -p \"require('node-addon-api').include\")",
                "../../core/include/webview",
                "./src"
            ],
            'dependencies': ["<!(node -p \"require('node-addon-api').targets\"):node_addon_api"],
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
        },
        {
            'target_name': 'jscallback',
            'defines': ['V8_DEPRECATION_WARNINGS=1'],
            'sources': ['src/JsCallback.cc'],
            'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")", "./src"],
            'dependencies': ["<!(node -p \"require('node-addon-api').targets\"):node_addon_api_except_all"],
            'cflags!': ['-fno-exceptions'],
            'cflags_cc!': ['-fno-exceptions', '-fno-rtti'],
            "cflags": ["-O2", "-fpermissive"],
            'conditions': [
                ['OS=="mac"', {
                    'cflags+': ['-fvisibility=hidden'],
                    'xcode_settings': {
                        'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
                    }
                }]
            ],
            'msvs_settings': {
                'VCCLCompilerTool': {'ExceptionHandling': 1},
            }
        }

    ]
}
