#ifndef FITSFILE_H_
#define FITSFILE_H_
#include <fitsio.h>
#include <iostream>
#include <cstddef>
//using namespace std;

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


  void open(const char* name);


  void create(const char* name);


  void close();



  
};




#endif
