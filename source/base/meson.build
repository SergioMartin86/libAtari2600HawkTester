# baseLibA2600Hawk Core sources

baseLibA2600HawkSrc =  [
]

baseLibA2600HawkIncludeDirs = [
 '.',
]

baseLibA2600HawkCompileArgs = [
  '-DSDL_SUPPORT',
  '-DUNIX',
  '-DBSPF_UNIX',
  '-DWINDOWED_SUPPORT',
  '-D_REENTRANT',
    '-Wno-multichar'
]

# baseLibA2600Hawk Core Configuration

 baseLibA2600HawkDependency = declare_dependency(
  compile_args        : [  baseLibA2600HawkCompileArgs ],
  include_directories : include_directories(baseLibA2600HawkIncludeDirs),
  sources             : [ baseLibA2600HawkSrc ],
  dependencies        : [  
                          dependency('sdl2',  required : true),
                        ]
 )