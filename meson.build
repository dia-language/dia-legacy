project('dia', 'c')

src = ['main.c', 'dia.c', 'dia_calculation.c', 'dia_vector.c', 'dia.tab.c', 'lex.yy.c']
bison = find_program('bison', required: true)
flex = find_program('flex')

if bison.found()
  run_command(bison, '-d', 'dia.y')
endif

if flex.found()
  run_command(flex, 'dia.l')
endif

executable('diac', src)
