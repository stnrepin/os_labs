$src = @('../actions.c', '../error.c', '../main.c', '../menu.c')
$cargs = @('/O2', '/GL', '/Fa', '/arch:AVX2', '/std:c11', '/openmp', '-I', '../', '-DUNICODE')

cl.exe @cargs @src /Fe:main.exe
