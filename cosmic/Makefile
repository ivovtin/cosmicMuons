all:    cosmic

LIBS	= $(shell root-config --libs --glibs) -lGui -lMinuit -lNew -L/space/KEDR/stable/lib -lKDB -lpq -lcrypt -lm -lstdc++
CERNLIBS	= `cernlib mathlib` -lg2c

INC	= $(shell root-config --cflags) -I/space/KEDR/stable/ -I/cern/pro/include/cfortran/ -Df2cFortran

clean:
	rm -f cosmic.o cosmic

cosmic:	cosmic.C
	cc -o cosmic cosmic.C $(LIBS) $(INC)


