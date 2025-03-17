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
            'dependencies': ["<!(node -p \"require('node-addon-api').targets\"):node_addon_api_except_all"],

            'conditions': [

                ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"',
                 {
                     'include_dirs': [
                         " <!(pkg-config --cflags gtk4 webkitgtk-6.0)"
                     ],
                     'libraries': [" <!(pkg-config --libs gtk4 webkitgtk-6.0)", "-ldl"],
                     'cflags': ["-std=c++17", "-O2"],
                 }
                 ],
                ['OS=="mac"',
                 {
                     'xcode_settings': {
                         'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
                         "OTHER_LDFLAGS": ["-framework", "WebKit", "-ldl"],
                     },
                     "cflags_cc": ["-std=c++17", "-O2"]
                 }
                 ],
                ['OS=="win"', {
                    "defines": [
                        "WEBVIEW_MSWEBVIEW2_BUILTIN_IMPL=0",
                        "WEBVIEW_MSWEBVIEW2_EXPLICIT_LINK=0",
                        "WEBVIEW_STATIC=1"
                    ],
                    'variables': {
                        'WV2_INCLUDE%': './src/Microsoft.Web.WebView2.<!(python ./src/get_mswv2_version.py)/build/native/include'
                    },
                    "conditions": [
                        ['target_arch=="x64"', {
                            'variables': {
                                'WV2_LIB%': './src/Microsoft.Web.WebView2.<!(python ./src/get_mswv2_version.py)/build/native/x64'
                            }}],
                        ['target_arch=="ia32"', {
                            'variables': {
                                'WV2_LIB%': './src/Microsoft.Web.WebView2.<!(python ./src/get_mswv2_version.py)/build/native/x86'
                            }}],
                        ['target_arch=="arm64"', {
                            'variables': {
                                'WV2_LIB%': './src/Microsoft.Web.WebView2.<!(python ./src/get_mswv2_version.py)/build/native/arm64'
                            }}]
                    ],
                    'msbuild_settings': {
                        'ClCompile': {
                            'LanguageStandard': 'stdcpp17'
                        }
                    },
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            "Optimization": 2,  # -O2
                            # -static (/MT Multi-threaded static runtime).
                            "RuntimeLibrary": 0,
                        }
                    },
                    'include_dirs': [
                        "<(WV2_INCLUDE)",
                    ],
                    "libraries": [
                        "advapi32.lib",
                        "ole32.lib",
                        "shell32.lib",
                        "shlwapi.lib",
                        "user32.lib",
                        "version.lib",
                        "../<(WV2_LIB)/WebView2LoaderStatic.lib"
                    ]
                }]
            ]
        },
        {
            'target_name': 'jscallback',
            'defines': ['V8_DEPRECATION_WARNINGS=1'],
            'sources': ['src/JsCallback.cc'],
            'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")", "./src"],
            'dependencies': ["<!(node -p \"require('node-addon-api').targets\"):node_addon_api_except_all"],
            'conditions': [
                ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"',
                 {
                     'cflags': ["-std=c++17", "-O2"],
                 }
                 ],
                ['OS=="mac"', {
                    'xcode_settings': {
                        'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
                    },
                    'cflags': ["-std=c++17", "-O2"],
                }],
                ['OS=="win"', {
                    'msbuild_settings': {
                        'ClCompile': {
                            'LanguageStandard': 'stdcpp17',
                        }
                    },
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            "Optimization": 2,
                            "RuntimeLibrary": 0,
                        },
                    },
                }]
            ],

        }
    ]
}
