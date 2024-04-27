# Check the core has been built

message('Running Dotnet publish for LibAtari2600Hawk...')
run_command('dotnet', 'publish', 'core/LibAtari2600Hawk.sln', '-r', 'linux-x64', check: true)

# baseLibA2600Hawk Core sources

baseLibA2600HawkSrc =  [
]

baseLibA2600HawkIncludeDirs = [
 '.',
 'core/output/linux-x64/publish/include/'
]

baseLibA2600HawkCompileArgs = [
]

# baseLibA2600Hawk Core Configuration

srcDir = meson.current_source_dir()

baseLibA2600HawkDependency = declare_dependency(
compile_args        : [  baseLibA2600HawkCompileArgs ],
include_directories : include_directories(baseLibA2600HawkIncludeDirs),
link_args           : [ srcDir + '/core/output/linux-x64/publish/libAtari2600Hawk.so' ],
sources             : [ baseLibA2600HawkSrc ],
dependencies        : [  
                        dependency('sdl2',  required : true),
                      ]
)