import os

LIBSTROPHE_BASE='../libstrophe'
if ARGUMENTS.get('LIBSTROPHE_BASE') is not None :
	LIBSTROPHE_BASE=ARGUMENTS.get('LIBSTROPHE_BASE')

INC_PATH=[LIBSTROPHE_BASE, 'include']
LIB_PATH=[LIBSTROPHE_BASE + '/.libs/', '.']

sources = Glob('source/*.c')
env = Environment()
env.AppendUnique(CPPPATH = INC_PATH, CCFLAGS = ['-Wall', '-Werror', '-Wno-unused'])
sharedlib = env.SharedLibrary('wksxmppxep', sources)
staticlib = env.StaticLibrary('wksxmppxep', sources)


menv = Environment()
menv.AppendUnique(CPPPATH = INC_PATH, CCFLAGS = ['-Wall', '-Werror'])
menv.AppendUnique(LIBS = ['wksxmppxep', 'strophe', 'ssl', 'crypto', 'expat'])
menv.AppendUnique(LIBS = ['pthread', 'm'])
menv.AppendUnique(LIBPATH = LIB_PATH, RPATH = LIB_PATH)
#menv.Program('example/main.c')
menv.Program('example/main_chat.c')
menv.Program('example/main_ibb.c')
menv.Program('example/main_ibb_data.c')

