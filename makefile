IFITS =-I/usr/local/include 
LFITS =-L/usr/local/lib
HEADS= fits_trait.h fitsfile.h fio.h 
BASOBJ=fits_trait.o fio.o region.o region_imp.o fitsfile.o


CPPFLAGS=-O3

target= libfio.a libfio.so fits2txt txt2fits
all:	${target}

region.o:region.cc region.h region_imp.h
	g++ $(CPPFLAGS) -c region.cc -fPIC

region_imp.o:region_imp.cc region_imp.h region.h
	g++ $(CPPFLAGS) -c region_imp.cc -fPIC


fits_trait.o:fits_trait.cc fits_trait.h
	g++ $(CPPFLAGS) -g -c fits_trait.cc ${IFITS} -fPIC

fitsfile.o:fitsfile.cc fitsfile.h
	g++ $(CPPFLAGS) -g -c fitsfile.cc ${IFITS} -fPIC

fio.o:fio.cc fio.h
	g++ $(CPPFLAGS) -g -c fio.cc -I ${IFITS} -fPIC


libfio.a:libfio.o ${BASOBJ}
	ar rv $@ libfio.o ${BASOBJ}

libfio.so:libfio.o ${BASOBJ}
	g++ -fPIC libfio.o ${BASOBJ} --shared -o $@

libfio.o:libfio.cc ${HEADS}
	g++ -o $@ -c $< ${IFITS}  -fPIC


fits2txt:fits2txt.o ${BASOBJ}
	g++ $< ${LFITS} -L ./  -lfio -lcfitsio -o $@

fits2txt.o:fits2txt.cc
	g++ $(CPPFLAGS) -g -c $< ${IFITS}

txt2fits:txt2fits.o ${BASOBJ}
	g++ $< ${LFITS} -L ./  -lfio -lcfitsio -o $@

txt2fits.o:txt2fits.cc
	g++ $(CPPFLAGS) -g -c $< ${IFITS}


install:
	cp libfio.a /usr/local/lib/
	cp libfio.so /usr/local/lib/
	cp *.h /usr/local/include/
#	ln -sf /usr/local/lib/libfio.so /usr/lib/

clean:
	rm -rf ${target}
	rm -rf *.o
