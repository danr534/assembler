utils.o: src/utils.c include/utils.h
first_pass.o: src/first_pass.c include/first_pass.h \
 include/pre_assembler.h include/globals.h include/macros_list.h \
 include/utils.h
assembler.o: src/assembler.c include/first_pass.h include/pre_assembler.h \
 include/globals.h include/macros_list.h include/utils.h \
 include/pre_assembler.h include/utils.h include/globals.h
macros_list.o: src/macros_list.c include/macros_list.h
pre_assembler.o: src/pre_assembler.c include/pre_assembler.h \
 include/globals.h include/macros_list.h include/utils.h
