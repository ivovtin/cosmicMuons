all:    kp_cosmic

LIBS	= $(shell root-config --libs --glibs) -L/space/KEDR/stable/lib -lKDB -lpq -lcrypt -lm -lstdc++
CERNLIBS	= `cernlib mathlib` -lg2c

INC	= $(shell root-config --cflags) -I/space/KEDR/stable/ -I/cern/pro/include/cfortran/ -Df2cFortran

clean:
	rm -f kp_cosmic.o kp_cosmic

kp_cosmic:	kp_cosmic.C
	cc -o kp_cosmic kp_cosmic.C $(LIBS) $(INC)


