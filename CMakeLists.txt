project(dia)

find_package(BISON)
find_package(FLEX)

BISON_TARGET(dia_parser dia.y ${PROJECT_SOURCE_DIR}/dia.tab.c)
FLEX_TARGET(dia_scanner dia.l ${PROJECT_SOURCE_DIR}/lex.yy.c)
ADD_FLEX_BISON_DEPENDENCY(dia_scanner dia_parser)

add_executable(diac
  main.c
  dia.c
  dia_calculation.c
  dia_vector
  ${BISON_dia_parser_OUTPUTS}
  ${FLEX_dia_scanner_OUTPUTS}
)

