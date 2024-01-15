project('porkchop', 'cpp',
        default_options : [
                'cpp_std=c++14',
                'warning_level=2',
                'werror=true']
)

# warning_levels:
# level 1: -Wall
# level 2: -Wall -Wextra
# level 3: -Wall -Wextra -Wpedantic
# so we need some more
warning_flags = [
        '-Wshadow',
        '-Wno-unused-parameter'
]

main_target_name = 'porkchop'

cxx = meson.get_compiler('cpp')
add_project_arguments(cxx.get_supported_arguments(warning_flags), language : 'cpp')

# sources
srcs = [
        'src/main.cpp',
        'src/porkchop.cpp'
]

# headers
hdr_dirs = [
        'include'
]

# dependencies
# json is just headers
json_inc = include_directories('third_party/json/nlohmann')

deps = [
        dependency('keplerian_toolbox'),
        declare_dependency(include_directories: json_inc)
]

executable(main_target_name, srcs,
        include_directories: hdr_dirs,
        dependencies: deps)

