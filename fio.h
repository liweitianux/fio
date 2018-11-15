#ifndef FIO_H
#define FIO_H

#include <blitz/array.h>
#include <iostream>
#include <fitsio.h>
#include <cstddef>

/* "fitsfile.h" begins */

class cfitsfile
{
 private:
  fitsfile* pff;

 private:
  cfitsfile(const cfitsfile&);
  cfitsfile& operator=(const cfitsfile&);

 public:
  cfitsfile();
  ~cfitsfile();

 public:
  fitsfile*& fptr();
  int open(const char* name);
  int create(const char* name);
  void close();
};

/* "fitsfile.h" ends */

extern fitsfile* openimage(const char* name);
extern fitsfile* createfitsfile(const char* name);
extern void closefitsfile(fitsfile*& ff);
template <typename T>
extern void operator>>(fitsfile*& ff,::blitz::Array<T,2>& h2d);
template <typename T>
extern void get_all_col(cfitsfile& ff,::blitz::Array<T,1>& h1d,const char* colname);
template <typename T>
extern void get_all_col(cfitsfile& ff,::blitz::Array<T,1>& h1d,int colnum);
extern int get_col_num(cfitsfile& ff,const char* colname);
extern long get_num_rows(cfitsfile& ff);
template <typename T>
extern void operator>>(cfitsfile& ff,::blitz::Array<T,2>& h2d);
template <typename T>
extern void operator<<(fitsfile* ff,::blitz::Array<T,2>& h2d);
template <typename T>
extern void operator<<(cfitsfile& ff,::blitz::Array<T,2>& h2d);
extern double get_x_offset(cfitsfile& ff);
extern double get_y_offset(cfitsfile& ff);
extern void put_offsets(cfitsfile& ff,double x_offset,double y_offset);
extern void pass_offsets(cfitsfile& ff1,cfitsfile& ff2);


/* "fits_trait.h" begins */

template <typename T>
class fits_trait;

template <>
class fits_trait<short>
{
 public:
  const static int bitpix=SHORT_IMG;
  const static int datatype=TSHORT;
  static short nulval;
  static short max;
  static short min;
};


template <>
class fits_trait<long>
{
 public:
  const static int bitpix=LONG_IMG;
  const static int datatype=TLONG;
  static long nulval;
  static long max;
  static long min;
};

template <>
class fits_trait<float>
{
 public:
  const static int bitpix=FLOAT_IMG;
  const static int datatype=TFLOAT;
  static float nulval;
  static float max;
  static float min;
};

template <>
class fits_trait<double>
{
 public:
  const static int bitpix=DOUBLE_IMG;
  const static int datatype=TDOUBLE;
  static double nulval;
  static double max;
  static double min;
};

#ifdef WITH_COMPLEX
template <>
class fits_trait<std::complex<float> >
{
 public:
  const static int bitpix=FLOAT_IMG;
  const static int datatype=TCOMPLEX;
  static std::complex<float> nulval;
  //static float max;
  //static float min;
};

template <>
class fits_trait<std::complex<double> >
{
 public:
  const static int bitpix=DOUBLE_IMG;
  const static int datatype=TDBLCOMPLEX;
  static std::complex<double> nulval;
  //static double max;
  //static double min;
};
#endif

/* "fits_trait.h" ends */


template <int typecode>
class typecode_trait;

template <>
class typecode_trait<TSHORT>
{
 public:
  typedef short datatype;
};

template <>
class typecode_trait<TLONG>
{
 public:
  typedef long datatype;
};

template <>
class typecode_trait<TFLOAT>
{
 public:
  typedef float datatype;
};

template <>
class typecode_trait<TDOUBLE>
{
 public:
  typedef double datatype;
};

#ifdef WITH_COMPLEX
template <>
class typecode_trait<TCOMPLEX>
{
 public:
  typedef std::complex<float> datatype;
};

template <>
class typecode_trait<TDBLCOMPLEX>
{
 public:
  typedef std::complex<double> datatype;
};
#endif
template <typename T>
void get_all_col(cfitsfile& ff,::blitz::Array<T,1>& h1d,const char* colname)
{
  int status=0;
  int hdutype;
  int anynul;
  int nulval;
  int nrows=get_num_rows(ff);
  T* data=new T[nrows];
 
  if(ffmahd(ff.fptr(),2,&hdutype,&status))
    {
      std::cerr<<"fdsfsd";
      std::cerr<<"error:"<<status<<std::endl;
    }
  
  if(ffgcv(ff.fptr(),fits_trait<T>::datatype,get_col_num(ff,const_cast<char*>(colname)),1,1,nrows,
	   &nulval,data,&anynul,&status))
    {
      std::cerr<<"fdsf"<<std::endl;
      std::cerr<<"error:"<<status<<std::endl;
    }
  ::blitz::Array<T,1> temp(data, ::blitz::shape(nrows),::blitz::deleteDataWhenDone);
  h1d.resize(nrows);
  h1d=temp;
  //  cout<<h1d.extent(0)<<endl;
}


template <typename T>
void get_all_col(cfitsfile& ff,blitz::Array<T,1>& h1d,int colnum)
{
  int status=0;
  int hdutype;
  int anynul;
  
  int nrows=get_num_rows(ff);
  T* data=new T[nrows];
 
  if(ffmahd(ff.fptr(),2,&hdutype,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }
  if(ffgcv(ff.fptr(),fits_trait<T>::datatype,colnum,1,1,nrows,
	   &fits_trait<T>::nulval,data,&anynul,&status))
    {
      std::cerr<<"fdsf"<<std::endl;
      std::cerr<<"error:"<<status<<std::endl;
    }
  ::blitz::Array<T,1> temp(data, ::blitz::shape(nrows),::blitz::deleteDataWhenDone);
  h1d.resize(nrows);
  h1d=temp;
}


template <typename T>
void operator>>(fitsfile*& ff,::blitz::Array<T,2>& h2d)
{
  T* p;
  long fpixel[2];
  long dims[2];
  int status;
  int hdutype;
  T nullval;
  int anynull;
  status=0;
  if(fits_movabs_hdu(ff,1,&hdutype,&status))
    {
      std::cerr<<"error!"<<status<<std::endl;
      exit(-1);
    }
  if(ffgisz(ff,2,dims,&status))
    {
      std::cerr<<"error!"<<status<<std::endl;
      exit(-1);
    }
  p=new T[dims[1]*dims[0]];
  nullval=0;
  anynull=1;
  status=0;
  fpixel[0]=fpixel[1]=1;
  if(ffgpxv(ff,fits_trait<T>::datatype,fpixel,dims[1]*dims[0],&nullval,p,&anynull,&status))
    {
      std::cerr<<"error!"<<status<<std::endl;
      exit(-1);
    }
  ::blitz::Array<T,2> temp(p,::blitz::shape(dims[1],dims[0]),::blitz::deleteDataWhenDone);
  h2d.resize(dims[1],dims[0]);
  h2d=temp;
}

template <typename T>
void operator>>(cfitsfile& ff,::blitz::Array<T,2>& h2d)
{
  ff.fptr()>>h2d;
}

template <typename T>
void operator>>(cfitsfile& ff,::blitz::Array<T,3>& h2d)
{
  T* p;
  long fpixel[3];
  long dims[3];
  int status;
  int hdutype;
  T nullval;
  int anynull;
  status=0;
  if(fits_movabs_hdu(ff.fptr(),1,&hdutype,&status))
    {
      std::cerr<<"error!"<<status<<std::endl;
      exit(-1);
    }
  if(ffgisz(ff.fptr(),3,dims,&status))
    {
      std::cerr<<"error!"<<status<<std::endl;
      exit(-1);
    }
  p=new T[dims[1]*dims[0]*dims[2]];
  nullval=0;
  anynull=1;
  status=0;
  fpixel[0]=fpixel[1]=fpixel[2]=1;
  if(ffgpxv(ff.fptr(),fits_trait<T>::datatype,fpixel,dims[1]*dims[0]*dims[2],&nullval,p,&anynull,&status))
    {
      std::cerr<<"error!"<<status<<std::endl;
      exit(-1);
    }
  ::blitz::Array<T,3> temp(p,::blitz::shape(dims[2],dims[1],dims[0]),::blitz::deleteDataWhenDone);
  h2d.reference(temp);
}

template <typename T>
void operator<<(fitsfile* ff,::blitz::Array<T,2>& h2d)
{
  long naxes[2];
  naxes[0]=h2d.extent(1);
  naxes[1]=h2d.extent(0);
  int status=0;
  T* data;

  data=h2d.data();
  
  if(fits_create_img(ff,fits_trait<T>::bitpix,2,naxes,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }
  if(fits_write_img(ff,fits_trait<T>::datatype,1,h2d.extent(0)*h2d.extent(1),data,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }
  
  char mtype1[]="sky";
  if(fits_update_key(ff,TSTRING,"MTYPE1",mtype1,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }

  char mform1[]="x,y";
  if(fits_update_key(ff,TSTRING,"MFORM1",mform1,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }

  char ctype1p[]="x";
  if(fits_update_key(ff,TSTRING,"CTYPE1P",ctype1p,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }
  
  double ltv1=0;
  if(fits_update_key(ff,TDOUBLE,"LTV1",&ltv1,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }

  char ctype2p[]="y";
  if(fits_update_key(ff,TSTRING,"CTYPE2P",ctype2p,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }
  
  double ltv2=0;
  if(fits_update_key(ff,TDOUBLE,"LTV2",&ltv2,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }

  double crval1p=0;
  if(fits_update_key(ff,TDOUBLE,"CRVAL1P",&crval1p,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }

  double crval2p=0;
  if(fits_update_key(ff,TDOUBLE,"CRVAL2P",&crval2p,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }
 
  double crpix1p=0.5;
   if(fits_update_key(ff,TDOUBLE,"CRPIX1P",&crpix1p,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }
   
   double crpix2p=0.5;
   if(fits_update_key(ff,TDOUBLE,"CRPIX2P",&crpix2p,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }

   double cdelt1p=1;
   if(fits_update_key(ff,TDOUBLE,"CDELT1P",&cdelt1p,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }

   double cdelt2p=1;
   if(fits_update_key(ff,TDOUBLE,"CDELT2P",&cdelt2p,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }

   char wcsty1p[]="PHYSICAL";
   if(fits_update_key(ff,TSTRING,"WCSTY1P",wcsty1p,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }

   char wcsty2p[]="PHYSICAL";
   if(fits_update_key(ff,TSTRING,"WCSTY2P",wcsty2p,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }
   
   double ltm1_1=1;
   if(fits_update_key(ff,TDOUBLE,"LTM1_1",&ltm1_1,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }

   double ltm2_2=1;
   if(fits_update_key(ff,TDOUBLE,"LTM2_2",&ltm2_2,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }
}




template <typename T>
void operator<<(cfitsfile& ff,::blitz::Array<T,2>& h2d)
{
  //  operator<<(ff.fptr(),h2d);
  ff.fptr()<<h2d;
}


template <typename T>
void operator<<(cfitsfile& cff,::blitz::Array<T,3>& h2d)
{
  long naxes[3];
  naxes[2]=h2d.extent(0);
  naxes[1]=h2d.extent(1);
  naxes[0]=h2d.extent(2);
  //dims[2],dims[1],dims[0]
  int status=0;
  T* data;

  data=h2d.data();
  fitsfile* ff=cff.fptr();
  if(fits_create_img(ff,fits_trait<T>::bitpix,3,naxes,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }
  if(fits_write_img(ff,fits_trait<T>::datatype,1,h2d.extent(0)*h2d.extent(1)*h2d.extent(2),data,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }
  
  char mtype1[]="sky";
  if(fits_update_key(ff,TSTRING,"MTYPE1",mtype1,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }

  char mform1[]="x,y";
  if(fits_update_key(ff,TSTRING,"MFORM1",mform1,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }

  char ctype1p[]="x";
  if(fits_update_key(ff,TSTRING,"CTYPE1P",ctype1p,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }
  
  double ltv1=0;
  if(fits_update_key(ff,TDOUBLE,"LTV1",&ltv1,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }

  char ctype2p[]="y";
  if(fits_update_key(ff,TSTRING,"CTYPE2P",ctype2p,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }
  
  double ltv2=0;
  if(fits_update_key(ff,TDOUBLE,"LTV2",&ltv2,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }

  double crval1p=0;
  if(fits_update_key(ff,TDOUBLE,"CRVAL1P",&crval1p,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }

  double crval2p=0;
  if(fits_update_key(ff,TDOUBLE,"CRVAL2P",&crval2p,NULL,&status))
    {
      std::cerr<<"error:"<<status<<std::endl;
    }
 
  double crpix1p=0.5;
   if(fits_update_key(ff,TDOUBLE,"CRPIX1P",&crpix1p,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }
   
   double crpix2p=0.5;
   if(fits_update_key(ff,TDOUBLE,"CRPIX2P",&crpix2p,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }

   double cdelt1p=1;
   if(fits_update_key(ff,TDOUBLE,"CDELT1P",&cdelt1p,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }

   double cdelt2p=1;
   if(fits_update_key(ff,TDOUBLE,"CDELT2P",&cdelt2p,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }

   char wcsty1p[]="PHYSICAL";
   if(fits_update_key(ff,TSTRING,"WCSTY1P",wcsty1p,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }

   char wcsty2p[]="PHYSICAL";
   if(fits_update_key(ff,TSTRING,"WCSTY2P",wcsty2p,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }
   
   double ltm1_1=1;
   if(fits_update_key(ff,TDOUBLE,"LTM1_1",&ltm1_1,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }

   double ltm2_2=1;
   if(fits_update_key(ff,TDOUBLE,"LTM2_2",&ltm2_2,NULL,&status))
     {
       std::cerr<<"error:"<<status<<std::endl;
     }
}

template <typename T_KEY>
T_KEY read_key(cfitsfile& ff,std::string key_name)
{
  T_KEY result;
  int status=0;
  fits_read_key(ff.fptr(),fits_trait<T_KEY>::datatype,key_name.c_str(),&result,NULL,&status);
  if(status)
    {
      std::cerr<<"ERROR, status="<<status;
      assert(0);
    }
  return result;
}

class pixel
{
public:
  int i,j;
  pixel(int a,int b)
    :i(a),j(b)
  {}

  pixel()
    :i(0),j(0)
  {}

  pixel left()
    {
      return pixel(i,j-1);
    }
  pixel right()
    {
      return pixel(i,j+1);
    }
  pixel up()
    {
      return pixel(i-1,j);
    }
  pixel down()
    {
      return pixel(i+1,j);
    }

  pixel leftup()
    {
      return pixel(i-1,j-1);
    }

  pixel rightup()
    {
      return pixel(i-1,j+1);
    }

  pixel leftdown()
    {
      return pixel(i+1,j-1);
    }

  pixel rightdown()
    {
      return pixel(i+1,j+1);
    }

  bool operator<(const pixel& rhs)const
    {
      return (i==rhs.i)?(j<rhs.j?true:false):(i<rhs.i?true:false);
    }

};
class double_pixel
{
public:
  double i,j;
  double_pixel(double a,double b)
    :i(a),j(b)
  {}

  double_pixel()
    :i(0),j(0)
  {}

};


#endif

