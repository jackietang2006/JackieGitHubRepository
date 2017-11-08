IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR) -DDEBUG -g
#CFLAGS=-I$(IDIR) -O2 -DNDEBUG

ODIR=../obj
LDIR =lib
SDIR=src
BDIR=../bin
LIBS=-lm

_DEPS = parserCommon.h networkParser.h \
	netflow_v9.h routerTemplateContainer.h netflowV9Container.h \
	networkV9TemplateContainer.h \
	utilities.h \
	Thread.h \
	NfDecodeThread.h \
	RawUdp.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ =  netflow_v9.o \
	routerTemplateContainer.o netflowV9Container.o \
	networkV9TemplateContainer.o \
	utilities.o \
	Thread.o \
	RawUdp.o

_UTILITIES_OBJ = utilities.o

_NF_OBJ = netflowDecoder.o
_PP_OBJ = netflowPreProc.o
_NFGEN_OBJ = nfGenerator.o
_NFGENUDP_OBJ = nfGeneratorUDP.o
_NFCOL_OBJ = nfCollector.o \
	NfDecodeThread.o
_NFRELAYAGENT_OBJ = nfRelayAgent.o \
	NfDecodeThread.o
_FifoMain_OBJ = FifoMain.o
_NFMONITOR_OBJ = nfMonitor.o


OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
UTILITIES_OBJ = $(patsubst %,$(ODIR)/%,$(_UTILITIES_OBJ))
NF_OBJ = $(patsubst %,$(ODIR)/%,$(_NF_OBJ))
PP_OBJ = $(patsubst %,$(ODIR)/%,$(_PP_OBJ))
NFGEN_OBJ = $(patsubst %,$(ODIR)/%,$(_NFGEN_OBJ))
NFGENUDP_OBJ = $(patsubst %,$(ODIR)/%,$(_NFGENUDP_OBJ))
NFCOL_OBJ = $(patsubst %,$(ODIR)/%,$(_NFCOL_OBJ))
NFRELAYAGENT_OBJ = $(patsubst %,$(ODIR)/%,$(_NFRELAYAGENT_OBJ))
FifoMain_OBJ = $(patsubst %,$(ODIR)/%,$(_FifoMain_OBJ))
NFMONITOR_OBJ = $(patsubst %,$(ODIR)/%,$(_NFMONITOR_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -std=gnu++0x -o $@ $< $(CFLAGS)

all: $(BDIR)/netflowDecoder $(BDIR)/netflowPreProc $(BDIR)/nfGenerator $(BDIR)/nfGeneratorUDP $(BDIR)/nfCollector $(BDIR)/nfRelayAgent $(BDIR)/FifoMain $(BDIR)/nfMonitor

$(BDIR)/netflowDecoder: $(NF_OBJ) $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS) -lstdc++  -lpcap -pthread

$(BDIR)/netflowPreProc: $(PP_OBJ) $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS) -lstdc++  -lpcap -pthread

$(BDIR)/nfGenerator: $(NFGEN_OBJ) $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS) -lstdc++  -lpcap -pthread

$(BDIR)/nfGeneratorUDP: $(NFGENUDP_OBJ) $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS) -lstdc++  -lpcap -pthread

$(BDIR)/nfCollector: $(NFCOL_OBJ) $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS) -lstdc++  -lpcap -pthread

$(BDIR)/nfRelayAgent: $(NFRELAYAGENT_OBJ) $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS) -lstdc++  -lpcap -pthread

$(BDIR)/FifoMain: $(FifoMain_OBJ) $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS) -lstdc++  -lpcap -pthread

$(BDIR)/nfMonitor: $(NFMONITOR_OBJ) $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS) -lstdc++  -lpcap -pthread


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
	rm -f $(BDIR)/* *~ core $(INCDIR)/*~ 
