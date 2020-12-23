$src = @('../error.c', '../launcher.c', '../main.c', '../reader_writer.c')
$cargs = @('/Zi', '/EHsc', '/std:c11', '-I', '../')

cl.exe @cargs @src /Fe:main.exe
