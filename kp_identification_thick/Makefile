all:    kp_cosmic npethr_misident sigma_year

LIBS	= $(shell root-config --libs --glibs) -L/space/KEDR/stable/lib -lKDB -lpq -lcrypt -lm -lstdc++
CERNLIBS	= `cernlib mathlib` -lg2c

INC	= $(shell root-config --cflags) -I/space/KEDR/stable/ -I/cern/pro/include/cfortran/ -Df2cFortran

clean:
	rm -f kp_cosmic.o kp_cosmic npethr_misident.o npethr_misident sigma_year.o sigma_year

kp_cosmic:	kp_cosmic.C
	cc -o kp_cosmic kp_cosmic.C $(LIBS) $(INC)

npethr_misident:       npethr_misident.C
	cc -o npethr_misident npethr_misident.C $(LIBS) $(INC)

sigma_year:     sigma_year.C
	cc -o sigma_year sigma_year.C $(LIBS) $(INC)

