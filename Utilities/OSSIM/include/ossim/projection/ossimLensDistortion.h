//*******************************************************************
// Copyright (C) 2000 ImageLinks Inc.
//
// LGPL
// 
// Author:  Garrett Potts (gpotts@imagelinks.com)
//
// Description:
//
// ossimLensDistortion
//*******************************************************************
//  $Id: ossimLensDistortion.h 9094 2006-06-13 19:12:40Z dburken $

#ifndef ossimLensDistortion_HEADER
#define ossimLensDistortion_HEADER
#include <ossim/base/ossim2dTo2dTransform.h>
class OSSIM_DLL ossimLensDistortion : public ossim2dTo2dTransform
{
public:
   ossimLensDistortion(const ossimDpt& callibrartedCenter=ossimDpt(0,0))
      :theCenter(callibrartedCenter)
   {
   }
   void setCenter(const ossimDpt& center)
   {
      theCenter = center;
   }
   virtual void forward(const ossimDpt& input,
                        ossimDpt& output) const
   {
      distort(input, output);
   }
   virtual void inverse(const ossimDpt& input,
                        ossimDpt& output)
   {
      undistort(input, output);
   }
   virtual void distort(const ossimDpt& input, ossimDpt& output)const=0;
   virtual void undistort(const ossimDpt& input, ossimDpt& output)const=0;

   virtual bool saveState(ossimKeywordlist& kwl,
                          const char* prefix = 0)const;
   
   virtual bool loadState(const ossimKeywordlist& kwl,
                          const char* prefix = 0);
   
protected:
   ossimDpt theCenter;

TYPE_DATA   
};
#endif
