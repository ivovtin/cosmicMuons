all:    light_coefficient light_coefficient_cnt_cor

LIBS	= $(shell root-config --libs --glibs) -L/space/KEDR/stable/lib -lKDB -lpq -lcrypt -lm -lstdc++
CERNLIBS	= `cernlib mathlib` -lg2c

INC	= $(shell root-config --cflags) -I/space/KEDR/stable/ -I/cern/pro/include/cfortran/ -Df2cFortran

clean:
	rm -f light_coefficient.o light_coefficient light_coefficient_cnt_cor.o light_coefficient_cnt_cor

light_coefficient:	light_coefficient.C
	cc -o light_coefficient light_coefficient.C $(LIBS) $(INC)

light_coefficient_cnt_cor:	light_coefficient_cnt_cor.C
	cc -o light_coefficient_cnt_cor light_coefficient_cnt_cor.C $(LIBS) $(INC)


