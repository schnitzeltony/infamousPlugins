#include<stdint.h>
#include<math.h>
#include<stdio.h>


#define PI 3.1415926535897932384626433832795

void cart2polar(float re, float im, float* mag, float* ang)
{
	*mag = sqrt(re*re + im*im);
	*ang = atan2(im,re);
}

void polar2cart(float mag, float ang, float* re, float* im)
{
	*re = mag*cos(ang);
	*im = mag*sin(ang);
}

//uses matched z transform to map poles/zeros to the z plane
void s2z(float res, float ims, float Ts, float* rez, float* imz)
{
	*rez = expf(res*Ts)*cos(ims*Ts);
	*imz = expf(res*Ts)*sin(ims*Ts);
}

void convolve(float* f, uint8_t nf, float* g, uint8_t ng, float* h)
{
	//good ol' fasioned convolution by the definition
	uint16_t t, tau;
	uint8_t ubound, lbound;
	for(t=0;t<ng+nf-1;t++)
	{
		h[t] = 0;
		lbound = ((1+(long)t-ng)<=0)?0:(t-ng+1);
		ubound = (t<(nf-1))?t:(nf-1);
		//printf("%i %i %i, ",t,lbound,ubound);
		for(tau=lbound;tau<=ubound;tau++)
		{
			h[t] += f[tau]*g[t-tau];
			//printf("%i %i;",tau, t-tau);
		}
		//printf("\n");
	}
}

//this recursive function stores the values in tmp until all calculations
//are complete and then writes them into g[]
//naturally this requires g[] to be of sufficient size
void _cip(float* f, uint8_t nf, float* g, uint8_t ng, uint8_t t)
{
	float tmp = 0;
	uint16_t tau;
	uint8_t ubound, lbound;
	lbound = ((1+(long)t-ng)<=0)?0:(t-ng+1);
	ubound = (t<(nf-1))?t:(nf-1);
	for(tau=lbound;tau<=ubound;tau++)
	{
		tmp += f[tau]*g[t-tau];
		//printf("%i %i;",tau, t-tau);
	}
	//printf("\n");
	if(t<ng+nf-1)
	{
		_cip(f,nf,g,ng,t+1);
	}
	g[t] = tmp;
}

//in this version the result will be stored into g[];
void convolve_in_place(float* f, uint8_t nf, float* g, uint8_t ng)
{
	_cip(f,nf,g,ng,0);
}


float magnitude(float* f, uint8_t nf, float Ts, float freq_hz)
{
	uint8_t i;
	float re, im, retmp, imtmp, ang;
	//z^-1 = e^(-jwT);
	//mag =  ||f[0] + f[1]e^(-jwT) + f[1]e^(-jw2T) +... f[n-1]e^(-jw(n-1)T)||
	re = f[0];
	im = 0;
	ang = 2*PI*freq_hz*Ts;
	for(i=1;i<nf;i++)
	{
		polar2cart(f[i], ang*i, &retmp, &imtmp);
		re += retmp;
		im += imtmp;
	}
	cart2polar(re,im,&retmp,&imtmp);
	return retmp;
}

void resample_zero(float rezero, float imzero, float oldTs, float newTs, float* re, float* im)
{
	float ang, mag;
	cart2polar(rezero,imzero,*ang,*mag);
	*ang *= newTs/oldTs;
	polar2cart(mag,ang,re,im);
}

void dzpuf2filter(float* rezeros, float*imzeros,  uint8_t nzeros, float* repoles, float* impoles,  uint8_t npoles, float unityfreq, float zpTs, float filterTs,  float* filternum, float* filterden)
{
	//this function is the one I'll use, we'll "resample" the zeros/poles in the digital domain
	uint16_t i,nn,nd;
	float a,b,c;
	float poly[3];
	float re, im;

	nn=1;
	filternum[0] = 1;//start with a 1 so that polynomial starts building
	//might need to worry about prewarp someday w' = 2/T *tan(wT/2)
	for(i=0;i<nzeros;i++)
	{
		resample_zero(rezeros[i],imzeros[i],zpTs,filterTs,&re,&im);
		if(imzeros[i])
		{//complex, adds pair
			poly[0] = 1;
			poly[1] = -2*re;
			poly[2] = re*re + im*im;
			
			convolve_in_place(poly,3,filternum,nn);
			nn+=2;
			//printf("zero is %f %fi, poly is %f %f %f \n",rezeros[i],imzeros[i],poly[0],poly[1],poly[2]);
		}
		else
		{//real, single zero
			poly[0] = 1;
			poly[1] = -re;
			
			convolve_in_place(poly,2,filternum,nn);
			nn+=1;
			//printf("zero is %f %fi, poly is %f %f \n",rezeros[i],imzeros[i],poly[0],poly[1]);
		}
	}

	nd=1;
	filterden[0] = 1;//start with a 1 so that polynomial starts building
	//might need to worry about prewarp someday w' = 2/T *tan(wT/2)
	for(i=0;i<npoles;i++)
	{
		resample_zero(repoles[i],impoles[i],zpTs,filterTs,&re,&im);
		if(impoles[i])
		{//complex, adds pair
			poly[0] = 1;
			poly[1] = -2*re;
			poly[2] = re*re + im*im;
			
			convolve_in_place(poly,3,filterden,nd);
			nd+=2;
			//printf("pole is %f %fi, poly is %f %f %f \n",repoles[i],impoles[i],poly[0],poly[1],poly[2]);
		}
		else
		{//real, single zero
			poly[0] = 1;
			poly[1] = -re;
			
			convolve_in_place(poly,2,filterden,nd);
			nd+=1;
			//printf("pole is %f %fi, poly is %f %f\n",repoles[i],impoles[i],poly[0],poly[1]);
		}
	}

	b = magnitude(filternum,nn,Ts,unityfreq);
	a = magnitude(filterden,nd,Ts,unityfreq);
	c = a/b;//inverse of magnitude
	//printf("%f %i %i\n",c,nn,nd);
//printf("%f %fz^-1 %fz^-2 %fz^-3 \n-----------------------------------------------\n%f %fz^-1 %fz^-2 %fz^-3\n\n",filternum[0], filternum[1],filternum[2],filternum[3],filterden[0],filterden[1],filterden[2],filterden[3]);

	for(i=0;i<nn;i++)
	{
		filternum[i]*=c;
//printf("%f %fz^-1 %fz^-2 %fz^-3 \n-----------------------------------------------\n%f %fz^-1 %fz^-2 %fz^-3\n\n",filternum[0], filternum[1],filternum[2],filternum[3],filterden[0],filterden[1],filterden[2],filterden[3]);
	}
}

void azpuf2filter(float* rezeros, float*imzeros,  uint8_t nzeros, float* repoles, float* impoles,  uint8_t npoles, float unityfreq, float Ts,  float* filternum, float* filterden)
{
	//gameplan is to convert zeros and poles to z plane, create residuals, convolve the polynomial coefficients, then set the gain to unity at the specified frequency
	uint8_t i,nn,nd;
	float a,b,c;
	float poly[3];
	float re, im;

	nn=1;
	filternum[0] = 1;//start with a 1 so that polynomial starts building
	//might need to worry about prewarp someday w' = 2/T *tan(wT/2)
	for(i=0;i<nzeros;i++)
	{
		s2z(rezeros[i],imzeros[i],Ts,&re,&im);
		if(imzeros[i])
		{//complex, adds pair
			poly[0] = 1;
			poly[1] = -2*re;
			poly[2] = re*re + im*im;
			
			convolve_in_place(poly,3,filternum,nn);
			nn+=2;
			//printf("zero is %f %fi, poly is %f %f %f \n",rezeros[i],imzeros[i],poly[0],poly[1],poly[2]);
		}
		else
		{//real, single zero
			poly[0] = 1;
			poly[1] = -re;
			
			convolve_in_place(poly,2,filternum,nn);
			nn+=1;
			//printf("zero is %f %fi, poly is %f %f \n",rezeros[i],imzeros[i],poly[0],poly[1]);
		}
	}

	nd=1;
	filterden[0] = 1;//start with a 1 so that polynomial starts building
	//might need to worry about prewarp someday w' = 2/T *tan(wT/2)
	for(i=0;i<npoles;i++)
	{
		s2z(repoles[i],impoles[i],Ts,&re,&im);
		if(impoles[i])
		{//complex, adds pair
			poly[0] = 1;
			poly[1] = -2*re;
			poly[2] = re*re + im*im;
			
			convolve_in_place(poly,3,filterden,nd);
			nd+=2;
			//printf("pole is %f %fi, poly is %f %f %f \n",repoles[i],impoles[i],poly[0],poly[1],poly[2]);
		}
		else
		{//real, single zero
			poly[0] = 1;
			poly[1] = -re;
			
			convolve_in_place(poly,2,filterden,nd);
			nd+=1;
			//printf("pole is %f %fi, poly is %f %f\n",repoles[i],impoles[i],poly[0],poly[1]);
		}
	}

	b = magnitude(filternum,nn,Ts,unityfreq);
	a = magnitude(filterden,nd,Ts,unityfreq);
	c = a/b;//inverse of magnitude
	//printf("%f %i %i\n",c,nn,nd);
//printf("%f %fz^-1 %fz^-2 %fz^-3 \n-----------------------------------------------\n%f %fz^-1 %fz^-2 %fz^-3\n\n",filternum[0], filternum[1],filternum[2],filternum[3],filterden[0],filterden[1],filterden[2],filterden[3]);

	for(i=0;i<nn;i++)
	{
		filternum[i]*=c;
//printf("%f %fz^-1 %fz^-2 %fz^-3 \n-----------------------------------------------\n%f %fz^-1 %fz^-2 %fz^-3\n\n",filternum[0], filternum[1],filternum[2],filternum[3],filterden[0],filterden[1],filterden[2],filterden[3]);
	}
}

//for testing
void main()
{
/*
	float a = -20000;
	float b = 0;
	float c,d;
	s2z(a,b,1.0/44100,&c,&d);
	printf("%f %f\n",c,d);
*/

  	float rezeros[] = {-3.3011e+04,1.1251e+03};
	float imzeros[] ={6.9032e+04,4.7615e+04};

	float repoles[] = {-1.1661e+03,-5.8331e+03};
	float impoles[] = {3.8233e+04,2.0143e+04};
 
	float b[5], a[5];//5 coeff == 4 roots!
	zpuf2filter(rezeros,imzeros,2,repoles,impoles,2,20000,1.0/44100,b,a);
	printf("  %f %fz^-1 %fz^-2 %fz^-3 %fz^-4 \n------------------------------------------------------------------\n  %f %fz^-1 %fz^-2 %fz^-3 %fz^-4\n",b[0], b[1],b[2],b[3],b[4],a[0],a[1],a[2],a[3],a[4]);

}