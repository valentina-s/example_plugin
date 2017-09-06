#include "full_tracing.h"
#include <iostream>
using namespace std;



bool full_tracing(V3DPluginCallback2 &callback, const V3DPluginArgList & input, V3DPluginArgList & output){
  unsigned char *pData1;
  //void *pData1=NULL;
  V3DLONG sz_data[4];
  V3DLONG *sz_data_address[4] ;
  sz_data_address[4] = &sz_data[4];

  V3DLONG sz0,sz1,sz2,sz3;
  V3DLONG *sz0_address, *sz1_address, *sz2_address,*sz3_address;
  sz0_address = &sz0;
  sz1_address = &sz1;
  sz2_address = &sz2;
  sz3_address = &sz3;

  // adaptive_thresholding
  Image4DSimple image;
  Image4DSimple *p4DImage;
  p4DImage = &image;
  cout<<"address"<<p4DImage<<endl;
  adaptive_thresholding(callback, input, output, (unsigned char *)pData1, sz0_address,sz1_address,sz2_address,sz3_address,&image);

  //V3DLONG sz0 = *sz_data[0];
  //V3DLONG sz1 = *sz_data[1];
  //V3DLONG sz2 = *sz_data[2];
  //V3DLONG sz3 = *sz_data[3];

  int getDatatype = 1;

  cout<<sz0<<endl;
  cout<<sz1<<endl;
  cout<<sz2<<endl;
  cout<<sz3<<endl;

  //cout<<&pData1[0]<<endl;
  //cout<<unsigned(pData1[sz0*sz1*sz2*sz3-100])<<endl;
  cout<<"after adaptive thresholding"<<endl;
  cout<<p4DImage<<endl;
  cout<<image.getXDim()<<endl;
  cout<<image.getYDim()<<endl;
  cout<<image.getZDim()<<endl;

  unsigned char *pData = image.getRawData();




  unsigned int sum = 0;
  for (V3DLONG i=0; i<sz0*sz1*sz2;i++)
    sum = sum + unsigned(((unsigned char *)pData)[i]);
  cout<<"sum"<<unsigned(sum)<<endl;

  ////-------------------------------------------------------------------------------------------
  // Histogram equalization
  //-------------------------------------------------------------------------------------------
  printf("\nhistogram equalization ...\n");
  int lowerbound = 30, higherbound = 255;
  if (!hist_eq_range_uint8((unsigned char *)pData, sz0*sz1*sz2*sz3, lowerbound, higherbound))
  {
      v3d_msg("Error happens in proj_general_hist_equalization();\n");
      return(false);
  }
  cout<<"after histogram equalization"<<endl;
  sum = 0;
  for (V3DLONG i=0; i<sz0*sz1*sz2;i++)
    sum = sum + unsigned(((unsigned char *)pData)[i]);
  cout<<"sum"<<unsigned(sum)<<endl;
  //------------------------------------------------------------------------------------------
  // Rescaling/Thresholding
  //------------------------------------------------------------------------------------------
  rescale((unsigned char *) pData, sz0*sz1*sz2*sz3);

  // neuron tracing algorithm
  //-------------------------------------------------------------------------------------------
  // SIGEN Tracing
  //-------------------------------------------------------------------------------------------
  input_PARA PARA;
  vector<char *> *pinfiles = (input.size() >= 1) ? (vector<char *> *)input[0].p : 0;
  vector<char *> *pparas = (input.size() >= 2) ? (vector<char *> *)input[1].p : 0;
  vector<char *> infiles = (pinfiles != 0) ? *pinfiles : vector<char *>();
  vector<char *> paras = (pparas != 0) ? *pparas : vector<char *>();
  if (infiles.empty()) {
  	fprintf(stderr, "Need input image. \n");
  	return false;
  } else {
  	PARA.inimg_file = infiles[0];
  }
  int k = 0;
  PARA.channel = ((int)paras.size() >= k + 1) ? atoi(paras[k]) : 1;
  k++;
  neuron_tracing((unsigned char *)pData, sz0, sz1, sz2, sz3, 1,PARA,false);

 return(true);
}
