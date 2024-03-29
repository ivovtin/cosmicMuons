#--------------------------------------------------------------------------
# ��������� 3 ������ ����� �� �������
ifeq (,$(WORKDIR))
WORKDIR=..
endif

# �ӣ ��� �������� �������� ����� :) - ���������� �������� ��� ����
VERBOSEMAKE=""
# ���� ������ -g ��� ������ �� ����� - ���� ������ � ����
#NDEBUG=""

# ���������� ��� *.� ������
#CC_LOCAL := gcc
# ���������� ��� *.cc � *.cpp ������
#CXX_LOCAL := g++
# ���������� ��� *.f, *.for � *.F ������
F77_LOCAL := g77
# ������ ��� ����� ������ �� ���������
#LD_LOCAL := g++
# �����������������, ���� ���������� �������� ������ ��� �������
# ��������� � �����������. ������ �� ���������.
#LD_MULTI := ""

# �������������� ����� ��� ����������� C/C++
COPTS = -I. `root-config --cflags` #-ansi -pedantic
NOOPT = ""
# �������������� ����� ��� ����������� Fortran
#FOPTS = -I.
FOPTS  = -g -fvxt -Wall -fno-automatic -finit-local-zero \
-fno-second-underscore -ffixed-line-length-120 -Wno-globals \
-DCERNLIB_LINUX -DCERNLIB_UNIX -DCERNLIB_LNX -DCERNLIB_QMGLIBC -DCERNLIB_BLDLIB \
-DCOMPDATE="'$(COMPDATE)'" -I$(CERN)/pro/include -I$(CERN)/pro/include/geant321

# �������������� ����� ��� �������
##LDOPTS = -Xlinker -rpath -Xlinker `root-config --libdir`
LDOPTS = -lg2c `root-config --libs` /space/KEDR/stable/lib/libVDDCRec.a  -lMinuit

#���� ���������� ���������� ONLYBINARY, �� ���������� � ������ �����������
ONLYBINARY=""

# ���� ���������� �������� CERNLIB, �� ����� ��������������� ����
# ��������. ���������� ���������� ����������� � ����� �����. ��
# ��������� ����������� �������� ���������� jetset74 mathlib graflib
# geant321 grafX11 packlib
CERNLIBRARY = ""

# ������ ���������, ���� ��� �� �������� ����������� �����, �������
# ��������� �� ����������. � ����� ������ ���������� �����������������
# CERNLIBRARY
#CERNLIBS =
CERNLIBS = jetset74 mathlib graflib geant lapack3 blas packlib

# ��� ���������� ����������� �����
BINDIR := ./

# �������������� ���� (����������� ����� ���)
LIB_LOCAL= `root-config --libs` -lMinuit -lpq -lcrypt -lbz2 -ldl -lg2c

# ���������, ����� ��������� �� ����� ��������
BINARIES = analysis_cosmic

# ������, �� ����� ������� ����� ������ ��� �������
# (��� ������ �� ����� �������� � ����������)
# � ����� ���������� ���� ���������� ��� ������
analysis_cosmic_MODULES := analysis_cosmic
analysis_cosmic_LIBS := KaFramework KrAtc KDisplay VDDCRec KrVDDCMu KrMu \
KrdEdxPId KrDCCalibdEdx DchdEdxDataRoot VDDCRec KrToF KsToF KEmcRec LKrTools \
VDDCRec KsTrg KdConvert KrObjects KdDCSim FitTools DchGeom ReadNat KDB AppFramework


# ��������� ������ ����� �� �������
include $(WORKDIR)/KcReleaseTools/rules.mk

