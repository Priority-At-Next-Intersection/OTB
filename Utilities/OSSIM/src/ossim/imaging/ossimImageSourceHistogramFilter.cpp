//*******************************************************************
//
// License:  See top level LICENSE.txt file.
// 
// Author: Garrett Potts (gpotts@imagelinks)
// Description:
//
//*************************************************************************
// $Id: ossimImageSourceHistogramFilter.cpp 9963 2006-11-28 21:11:01Z gpotts $
#include <ossim/imaging/ossimImageSourceHistogramFilter.h>
#include <ossim/base/ossimHistogramSource.h>
#include <ossim/base/ossimMultiResLevelHistogram.h>
#include <ossim/base/ossimMultiBandHistogram.h>
#include <ossim/base/ossimHistogram.h>
#include <ossim/base/ossimCommon.h>
#include <ossim/imaging/ossimImageData.h>
#include <ossim/base/ossimKeyword.h>
#include <ossim/base/ossimKeywordlist.h>

static const ossimKeyword PROPRIETARY_FILENAME_KW("proprietary_filename",
                                                  "imports a proprietary file");

static const ossimKeyword HISTOGRAM_FILENAME_KW("histogram_filename",
                                                "read an OSSIM histogram file");


RTTI_DEF1(ossimImageSourceHistogramFilter, "ossimImageSourceHistogramFilter", ossimImageSourceFilter)
   
ossimImageSourceHistogramFilter::ossimImageSourceHistogramFilter()
   :ossimImageSourceFilter(),
    theCurrentResLevel(0),
    theHistogram(NULL),
    theFilename("")
{
   setNumberOfInputs(2);
   theInputListIsFixedFlag = true;
}

ossimImageSourceHistogramFilter::ossimImageSourceHistogramFilter(ossimImageSource* inputSource,
                                                                 ossimMultiResLevelHistogram* histogram)
   : ossimImageSourceFilter(inputSource),
     theCurrentResLevel(0),
     theHistogram(histogram),
     theFilename("")
{
   setNumberOfInputs(2);
   theInputListIsFixedFlag = true;
}

ossimImageSourceHistogramFilter::~ossimImageSourceHistogramFilter()
{
   if(theHistogram)
   {
      delete theHistogram;
      theHistogram = NULL;
   }   
}


void ossimImageSourceHistogramFilter::setHistogram(ossimMultiResLevelHistogram* histogram)
{
   if(theHistogram&&
      theHistogram!=histogram)
   {
      delete theHistogram;
      theHistogram = NULL;
      
   }
   theHistogram = histogram;
   
   if(theHistogram)
   {
      theHistogram->setBinCount(0, 0);
   }
}

bool ossimImageSourceHistogramFilter::setHistogram(const ossimFilename& filename)
{
   ossimMultiResLevelHistogram* histogram = new ossimMultiResLevelHistogram;

   bool result = histogram->importHistogram(filename);

   if(result)
   {
      theFilename = filename;
      setHistogram(histogram);
   }
   else
   {
      delete histogram;
      histogram = 0;
   }
   
   return result;
}

ossimMultiResLevelHistogram* ossimImageSourceHistogramFilter::getHistogram()
{
   if(!getInput(1))
   {
      return theHistogram;
   }
   else
   {
      ossimHistogramSource* histoSource = PTR_CAST(ossimHistogramSource, getInput(1));
      if(histoSource)
      {
         return histoSource->getHistogram();
      }
   }

   return (ossimMultiResLevelHistogram*)NULL;
}

const ossimMultiResLevelHistogram* ossimImageSourceHistogramFilter::getHistogram()const
{
   if(!getInput(1))
   {
      return theHistogram;
   }
   else
   {
      ossimHistogramSource* histoSource = PTR_CAST(ossimHistogramSource, getInput(1));
      if(histoSource)
      {
         return histoSource->getHistogram();
      }
   }

   return (ossimMultiResLevelHistogram*)NULL;
}

bool ossimImageSourceHistogramFilter::canConnectMyInputTo(ossim_int32 inputIndex,
                                                          const ossimConnectableObject* object)const
{
   if(object)
   {
      if(PTR_CAST(ossimImageSourceInterface, object) && ( inputIndex == 0 ))
      {
         return true;
      }
      else if(PTR_CAST(ossimHistogramSource, object) && ( inputIndex == 1 ))
      {
         return true;
      }
   }

   return false;
}

void ossimImageSourceHistogramFilter::connectInputEvent(ossimConnectionEvent& event)
{
   theInputConnection = PTR_CAST(ossimImageSourceInterface, getInput(0));
   if(PTR_CAST(ossimHistogramSource, event.getNewObject()))
   {
      // only initialize if it's a new object
      initialize();
   }
}

bool ossimImageSourceHistogramFilter::saveState(ossimKeywordlist& kwl,
                                                const char* prefix)const
{
   bool result = ossimImageSourceFilter::saveState(kwl, prefix);

   kwl.add(prefix,
           HISTOGRAM_FILENAME_KW,
           theFilename,
           true);
   
   return result;
}

bool ossimImageSourceHistogramFilter::loadState(const ossimKeywordlist& kwl,
                                                const char* prefix)
{
   const char* proprietaryName = kwl.find(prefix, PROPRIETARY_FILENAME_KW);
   const char* ossimName       = kwl.find(prefix, HISTOGRAM_FILENAME_KW);
   bool result = true;
   ossimFilename file;

   if(proprietaryName)
   {
       theFilename = ossimFilename(proprietaryName);
   }
   else if(ossimName)
   {
      if(!theHistogram)
      {
         theHistogram = new ossimMultiResLevelHistogram;
      }
      theFilename = ossimFilename(ossimName);
      
   }

   if(theFilename.exists()&&(theFilename!=""))
   {
      if(!theHistogram)
      {
         theHistogram = new ossimMultiResLevelHistogram;
      }
      result = theHistogram->importHistogram(theFilename);
   }
   if(theHistogram)
   {
      theHistogram->setBinCount(0, 0);
   }
   
   result =  ossimImageSourceFilter::loadState(kwl, prefix);

   setNumberOfInputs(2);
   theInputListIsFixedFlag = true;
   
   return result;
}

std::ostream& ossimImageSourceHistogramFilter::print(std::ostream& out) const
{

   out << "ossimImageSourceHistogramFilter::print:\n"
       << "theFilename:  " << endl;

   return ossimImageSourceFilter::print(out);
}
