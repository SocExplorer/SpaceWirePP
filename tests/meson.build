tests = [
    'rmap'
]

test_args = []
if get_option('teamcity')
    test_args += ['-r', 'teamcity']
endif

foreach test:tests
    exe = executable('test_'+test, test + '/main.cpp', dependencies:[catch_dep, SpaceWirePP_dep])
    test('test_'+test, exe, args: test_args)
endforeach
