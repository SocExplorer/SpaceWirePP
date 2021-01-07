project('SpaceWirePP', 'cpp',default_options : ['cpp_std=c++17'])

catch_dep = dependency('catch2', main : true)

SpaceWirePP_inc = include_directories(['include'])

SpaceWirePP_dep = declare_dependency(
  include_directories: SpaceWirePP_inc
)

subdir('tests')
