$src = @('../actions.c', '../error.c', '../main.c', '../menu.c')
$cargs = @('/Zi', '/EHsc', '/std:c11', '-I', '../')

cl.exe @cargs @src /Fe:main.exe
