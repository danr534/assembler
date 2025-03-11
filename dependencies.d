utils.o: src/utils.c include/utils.h
first_pass.o: src/first_pass.c include/first_pass.h \
 include/pre_assembler.h include/globals.h include/macros_list.h \
 include/utils.h include/labels_list.h
second_pass.o: src/second_pass.c include/second_pass.h \
 include/first_pass.h include/pre_assembler.h include/globals.h \
 include/macros_list.h include/utils.h include/labels_list.h \
 include/utils.h
assembler.o: src/assembler.c include/first_pass.h include/pre_assembler.h \
 include/globals.h include/macros_list.h include/utils.h \
 include/labels_list.h include/pre_assembler.h include/utils.h \
 include/globals.h include/second_pass.h include/first_pass.h
macros_list.o: src/macros_list.c include/macros_list.h
labels_list.o: src/labels_list.c include/labels_list.h
pre_assembler.o: src/pre_assembler.c include/pre_assembler.h \
 include/globals.h include/macros_list.h include/utils.h
