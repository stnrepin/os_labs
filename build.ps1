$src = @('../actions.c', '../error.c', '../main.c', '../menu.c')
$cargs = @('/Zi', '/EHsc', '/std:c11', '-I', '../', '-DUNICODE')

cl.exe @cargs @src
