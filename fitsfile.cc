#include "fitsfile.h"

cfitsfile::cfitsfile()
  :pff(NULL)
{}

cfitsfile::~cfitsfile()
{
  int status=0;
  if(pff)
    {
      if(fits_close_file(pff,&status))
	{
	  std::cerr<<"Error"<<status<<std::endl;
	}
    }
  pff=NULL;
}


fitsfile*& cfitsfile::fptr()
{
  return pff;
}

int cfitsfile::open(const char* name)
{
  int status=0;
  if(pff)
    {
      if(fits_close_file(pff,&status))
	{
	  std::cerr<<"Error:"<<status<<std::endl;
	  return status;
	}
    }
  if(ffopen(&pff,name,READWRITE,&status))
    {
      std::cerr<<"Error:"<<status<<std::endl;
    }
  return status;
}


int cfitsfile::create(const char* name)
{
  int status=0;
  this->close();
  remove(name);  
  if(ffinit(&pff,name,&status))
    {
      std::cerr<<"Error:"<<status<<std::endl;
    }
  return status;
}

void cfitsfile::close()
{
  int status=0;
  if(!pff)
    {
      return ;
    }
  if(fits_close_file(pff,&status))
    {
      std::cerr<<"Error"<<status<<std::endl;
    }
  pff=NULL;
}

