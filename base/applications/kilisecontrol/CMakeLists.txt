add_executable(kilisecontrol kilisecontrol.c)
set_module_type(kilisecontrol win32gui UNICODE)
add_importlibs(kilisecontrol user32 gdi32 shell32 msvcrt kernel32)
add_cd_file(TARGET kilisecontrol DESTINATION reactos/system32 FOR all)
