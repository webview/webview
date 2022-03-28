project ('webview', ['c', 'c++'])

compiler_c = meson.get_compiler('c')
compiler_cxx = meson.get_compiler('c++')

# platform specific dependenies
webview_dependencies = []
host_os = host_machine.system().to_lower()
if host_os.contains('windows')
  # add_project_arguments(['-I', join_paths(meson.source_root(), 'script')], language: 'c++')
  webview_dependencies = compiler_cxx.find_library('WebView2Loader')
elif host_os.contains('mingw') or host_os.contains('cygwin') 
  webview_dep = dependency('webkit2gtk-4.0', version: '>= 2.0')
  if webview_dep.found()
    webview_dependencies += webview_dep
  else
    # add_project_arguments(['-I', join_paths(meson.source_root(), 'script')], language: 'c++')
    webview_dependencies = compiler_cxx.find_library('WebView2Loader')
  endif

  add_project_arguments(['--define', 'WINDOWS'], language: 'c')
elif host_os.contains('linux')
  webview_dependencies += dependency('gtk+-3.0', version: '>= 4.4'),
  webview_dependencies += dependency('webkit2gtk-4.0', version: '>= 2.0')
  add_project_arguments(['--define', 'LINUX'], language: 'c')
elif host_os.contains('darwin') or host_os.contains('machten') or host_os.contains('rhapsody')
    webview_dependencies += dependency('gtk+-3.0', version: '>= 4.4'),
    webview_dependencies += dependency('webkit2gtk-4.0', version: '>= 2.0')
  add_project_arguments('-DMACOS', language: 'c')
endif

sources_webview = [
    webview.h,
    webview.cc
]

libwebview = library(
    'webview',
    sources_webview,
    dependencies: webview_dependencies,
    install: true,
)

pkgconfig.generate(
    libwebview,
    name: 'webview',
    filebase: 'webview',
    version: meson.project_version(),
    libraries: [ webview_dependencies ],
    description: 'Webview headers',
)

libwebview_dep = declare_dependency(
    link_with: libwebview,
    dependencies: webview_dependencies,
    include_directories: [include_directories('.')]
)

sources_execute = [
  'main.c',
]

header_webview = include_directories('.')

appkangaroo = executable(meson.project_name(),
  sources_execute,
  include_directories: [header_webview],
  dependencies: webview_dependencies,
  link_with: libwebview,
  install: true,
  win_subsystem: 'windows'
)