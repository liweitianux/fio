FITS_INC= 	-I./include
FITS_LIB= 	-L./lib
LINK_LIB= 	-lfio -lblitz -lcfitsio
HEADS= 		fio.h fits_trait.h fitsfile.h
OBJS= 		fio.o fits_trait.o fitsfile.o region.o region_imp.o

CXX?= 		g++
CXXFLAGS= 	-g -Wall -pipe -fPIC
LDFLAGS=	-Wl,-rpath='$$ORIGIN/lib'

TGT= 		lib/libfio.so fits2txt txt2fits


all: $(TGT)


lib/libfio.so: libfio.o $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $< $(OBJS) $(FITS_INC)


fits2txt: fits2txt.o lib/libfio.so lib/libblitz.so
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(FITS_LIB) $(LINK_LIB)


txt2fits: txt2fits.o lib/libfio.so lib/libblitz.so
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(FITS_LIB) $(LINK_LIB)


%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< $(FITS_INC)


lib/libblitz.so: blitz-0.9
	cd $<; ./configure --prefix=/tmp/$<-install \
	    --libdir=$(PWD)/lib --include=$(PWD)/include \
	    --disable-fortran --disable-doxygen \
	    --disable-dot --disable-html-docs \
	    --disable-static --enable-shared; \
	    make; make install; \
	    rm -rf /tmp/$<-install


blitz-0.9: blitz-0.9.tar.gz
	tar xf $<

clean:
	rm -f *.o $(TGT)
	rm -rf blitz-0.9 include/blitz include/random
	rm -rf lib/pkgconfig lib/libblitz.*


.PHONY: clean

#  vim: set ts=8 sw=4 tw=0 fenc=utf-8 ft=make: #
