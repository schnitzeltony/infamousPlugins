//Spencer Jackson
//resampler.cpp
#include"c_resampler.h"
#include"c_vresampler.h"
#include<zita-resampler/resampler.h>
#include<zita-resampler/vresampler.h>

// c wrapper for the excellent zita-resampler library
// to learn more about this library and its useage you can read the documentation
// at http://kokkinizita.linuxaudio.org/linuxaudio/zita-resampler/resampler.html

extern "C"{

#if(0)
typedef struct _Rsampler
{
    Resampler 
}Rsampler;
#endif

RESAMPLER_HANDLE ResamplerAlloc()
{
   Resampler* r = new Resampler();
   //r = (Resampler *)malloc(sizeof(Resampler));
   return (void *)r;
}

void ResamplerFree(RESAMPLER_HANDLE r)
{
   //free(static_cast<Resampler*>(r));
   delete[] static_cast<Resampler*>(r);
}

int ResamplerSetup(RESAMPLER_HANDLE r, unsigned int   fs_inp, unsigned int fs_out, unsigned int nchan, unsigned int hlen)
{
   return static_cast<Resampler*>(r)->setup(fs_inp,fs_out,nchan,hlen);
}

void ResamplerClear(RESAMPLER_HANDLE r)
{
   static_cast<Resampler*>(r)->clear();
}

int ResamplerReset(RESAMPLER_HANDLE r)
{
   return static_cast<Resampler*>(r)->reset();
}

int ResamplerProcess(RESAMPLER_HANDLE r)
{
   return static_cast<Resampler*>(r)->process();
}

int ResamperNChan(RESAMPLER_HANDLE r)
{
   return static_cast<Resampler*>(r)->nchan();
}

int ResamplerInpSize(RESAMPLER_HANDLE r)
{
   return static_cast<Resampler*>(r)->inpsize();
}

double ResamplerInpDist(RESAMPLER_HANDLE r)
{
   return static_cast<Resampler*>(r)->inpdist();
}


//access to public members
unsigned int ResamplerGetInpCount(RESAMPLER_HANDLE r)
{
   return static_cast<Resampler*>(r)->inp_count;
}

void ResamplerSetInpCount(RESAMPLER_HANDLE r, unsigned int count)
{
   static_cast<Resampler*>(r)->inp_count = count;
}

unsigned int ResamplerGetOutCount(RESAMPLER_HANDLE r)
{
   return static_cast<Resampler*>(r)->out_count;
}

void ResamplerSetInpCount(RESAMPLER_HANDLE r, unsigned int count)
{
   static_cast<Resampler*>(r)->out_count = count;
}

float* ResamplerGetInpData(RESAMPLER_HANDLE r)
{
   return static_cast<Resampler*>(r)->inp_data;

}

void ResamplerSetInpData(RESAMPLER_HANDLE r, float * data)
{
   static_cast<Resampler*>(r)->inp_data = data;
}

float* ResamplerGetOutData(RESAMPLER_HANDLE r)
{
   return static_cast<Resampler*>(r)->out_data;
}

void ResamplerSetOutData(RESAMPLER_HANDLE r, float * data)
{
   static_cast<Resampler*>(r)->out_data = data;
}






#if(0)
//VResampler Functions

typedef struct _VRsampler
{
    VResampler 
}VRsampler;

VRESAMPLER_HANDLE VResamplerAlloc(){
   VRsampler* r;
   r = (VResampler *)malloc(sizeof(VRsampler));
   return (void *)r;
}

void VResamplerFree(VRESAMPLER_HANDLE r)
{
    free(static_cast<VRsampler*>(r));
}

int VResamplerSetup(VRESAMPLER_HANDLE r, double ratio, unsigned int nchan, unsigned int hlen)
{
    return static_cast<VRsampler*>(r)->setup(ratio,nchan,hlen);
}

void VResamplerClear(VRESAMPLER_HANDLE r)
{
    return static_cast<VRsampler*>(r)->clear();
}

int VResamplerReset(VRESAMPLER_HANDLE r)
{
    return static_cast<VRsampler*>(r)->reset();
}

int VResamplerProcess(VRESAMPLER_HANDLE r)
{
    return static_cast<VRsampler*>(r)->process();
}

int VResamperNChan(VRESAMPLER_HANDLE r)
{
    return static_cast<VRsampler*>(r)->nchan();
}

int VResamplerInpSize(VRESAMPLER_HANDLE r)
{
    return static_cast<VRsampler*>(r)->inpsize();
}

double VResamplerInpDist(VRESAMPLER_HANDLE r)
{
    return static_cast<VRsampler*>(r)->inp_dist();
}

void VResamplerSetRRatio(VRESAMPLER_HANDLE r, double ratio)
{
    static_cast<VRsampler*>(r)->set_rratio(ratio);
}

void VResamplerSetRRFilt(VRESAMPLER_HANDLE r, double time)
{
    static_cast<VRsampler*>(r)->set_rrfilt(time);
}


//access to public members
unsigned int VResamplerGetInpCount(VRESAMPLER_HANDLE r)
{
    return static_cast<VRsampler*>(r)->inp_count;
}

void VResamplerSetInpCount(VRESAMPLER_HANDLE r, unsigned int count)
{
    static_cast<VRsampler*>(r)->inp_count = count;
}

unsigned int VResamplerGetOutCount(VRESAMPLER_HANDLE r)
{
    return static_cast<VRsampler*>(r)->out_count;
}

void VResamplerSetInpCount(VRESAMPLER_HANDLE r, unsigned int count)
{
    static_cast<VRsampler*>(r)->inp_count = count;
}

float* VResamplerGetInpData(VRESAMPLER_HANDLE r)
{
    return static_cast<VRsampler*>(r)->inp_data;
}

void VResamplerSetInpData(VRESAMPLER_HANDLE r, float * data)
{
    static_cast<VRsampler*>(r)->inp_data = data;
}

float* VResamplerGetOutData(VRESAMPLER_HANDLE r)
{
    return static_cast<VRsampler*>(r)->out_data;
}

void VResamplerSetOutData(VRESAMPLER_HANDLE r, float * data)
{
    //VRsampler* rs = static_cast<VRsampler*>(r);
    static_cast<VRsampler*>(r)->out_data = data;
}
#endif

}//extern c