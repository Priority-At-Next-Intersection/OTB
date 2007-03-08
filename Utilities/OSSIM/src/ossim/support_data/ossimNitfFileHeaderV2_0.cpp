//*******************************************************************
// Copyright (C) 2000 ImageLinks Inc. 
//
// License:  See top level LICENSE.txt file.
//
// Author: Garrett Potts
// 
// Description: Nitf support class
// 
//********************************************************************
// $Id: ossimNitfFileHeaderV2_0.cpp 10553 2007-02-28 20:35:26Z gpotts $


#include <sstream>
#include <cstring> // for memset
#include <iostream>
#include <iomanip>

#include <ossim/support_data/ossimNitfCommon.h>
#include <ossim/support_data/ossimNitfFileHeaderV2_0.h>
#include <ossim/support_data/ossimNitfImageHeaderV2_0.h>
#include <ossim/support_data/ossimNitfSymbolHeaderV2_0.h>
#include <ossim/support_data/ossimNitfLabelHeaderV2_0.h>
#include <ossim/support_data/ossimNitfTextHeaderV2_0.h>
#include <ossim/support_data/ossimNitfDataExtensionSegmentV2_0.h>

#include <ossim/base/ossimString.h>
#include <ossim/base/ossimTrace.h>
#include <ossim/base/ossimStringProperty.h>

const ossimString ossimNitfFileHeaderV2_0::FSDWNG_KW = "fsdwng";
const ossimString ossimNitfFileHeaderV2_0::FSDEVT_KW = "fsdevt";

static const ossimTrace traceDebug("ossimNitfFileHeaderV2_0:debug");

RTTI_DEF1(ossimNitfFileHeaderV2_0, "ossimNitfFileHeaderV2_0", ossimNitfFileHeaderV2_X)

std::ostream& operator <<(std::ostream& out,
                     const ossimNitfImageInfoRecordV2_0 &data)
{
   return out << "theImageSubheaderLength:       "
              << data.theImageSubheaderLength << std::endl
              << "theImageLength:                "
              << data.theImageLength;
}

ossim_int32 ossimNitfImageInfoRecordV2_0::getHeaderLength()const
{
   return ossimString(theImageSubheaderLength).toInt32();
}

ossim_int32  ossimNitfImageInfoRecordV2_0::getImageLength()const
{
   return ossimString(theImageLength).toInt32();
}

ossim_int32  ossimNitfImageInfoRecordV2_0::getTotalLength()const
{
   return (getHeaderLength() + getImageLength());
}

std::ostream& operator <<(std::ostream& out,
                     const ossimNitfSymbolInfoRecordV2_0 &data)
{
   return out << "theSymbolSubheaderLength:       "
              << data.theSymbolSubheaderLength << std::endl
              << "theSymbolLength:                "
              << data.theSymbolLength;
}
ossim_int32 ossimNitfSymbolInfoRecordV2_0::getHeaderLength()const
{
   return ossimString(theSymbolSubheaderLength).toInt32();
}

ossim_int32 ossimNitfSymbolInfoRecordV2_0::getImageLength()const
{
   return ossimString(theSymbolLength).toInt32();
}

ossim_int32 ossimNitfSymbolInfoRecordV2_0::getTotalLength()const
{
   return (getHeaderLength() + getImageLength());
}

std::ostream& operator <<(std::ostream& out,
                     const ossimNitfLabelInfoRecordV2_0 &data)
{
   return out << "theLabelSubheaderLength:       "
              << data.theLabelSubheaderLength << std::endl
              << "theLabelLength:                "
              << data.theLabelLength;
}

ossim_int32 ossimNitfLabelInfoRecordV2_0::getHeaderLength()const
{
   return ossimString(theLabelSubheaderLength).toInt32();
}

ossim_int32 ossimNitfLabelInfoRecordV2_0::getImageLength()const
{
   return ossimString(theLabelLength).toInt32();
}

ossim_int32 ossimNitfLabelInfoRecordV2_0::getTotalLength()const
{
   return (getHeaderLength() + getImageLength());
}

std::ostream& operator <<(std::ostream& out,
                     const ossimNitfTextInfoRecordV2_0 &data)
{
   return out << "theTextSubheaderLength:       "
              << data.theTextSubheaderLength << std::endl
              << "theTextLength:                "
              << data.theTextLength;
}

ossim_int32 ossimNitfTextInfoRecordV2_0::getHeaderLength()const
{
   return ossimString(theTextSubheaderLength).toInt32();
}

ossim_int32 ossimNitfTextInfoRecordV2_0::getImageLength()const
{
   return ossimString(theTextLength).toInt32();
}

ossim_int32 ossimNitfTextInfoRecordV2_0::getTotalLength()const
{
   return (getHeaderLength() + getImageLength());
}

std::ostream& operator <<(std::ostream& out,
                     const ossimNitfDataExtSegInfoRecordV2_0 &data)
{
   return out << "theDataExtSegSubheaderLength:       "
              << data.theDataExtSegSubheaderLength << std::endl
              << "theDataExtSegLength:                "
              << data.theDataExtSegLength;
}

ossim_int32 ossimNitfDataExtSegInfoRecordV2_0::getHeaderLength()const
{
   return ossimString(theDataExtSegSubheaderLength).toInt32();
}

ossim_int32 ossimNitfDataExtSegInfoRecordV2_0::getImageLength()const
{
   return ossimString(theDataExtSegLength).toInt32();
}

ossim_int32 ossimNitfDataExtSegInfoRecordV2_0::getTotalLength()const
{
   return (getHeaderLength() + getImageLength());
}

std::ostream& operator <<(std::ostream& out,
                     const ossimNitfResExtSegInfoRecordV2_0 &data)
{
   return out << "theResExtSegSubheaderLength:       "
              << data.theResExtSegSubheaderLength << std::endl
              << "theResExtSegLength:                "
              << data.theResExtSegLength;
}

ossimNitfFileHeaderV2_0::ossimNitfFileHeaderV2_0()
   :ossimNitfFileHeaderV2_X()
{
   clearFields();
}

ossimNitfFileHeaderV2_0::~ossimNitfFileHeaderV2_0()
{
}


void ossimNitfFileHeaderV2_0::parseStream(std::istream &in)
{
   if(traceDebug())
   {
      ossimNotify(ossimNotifyLevel_DEBUG)
         << "ossimNitfFileHeaderV2_0::parseStream:   entered ......."
         << std::endl;
   }
   
   clearFields();
   
   // identification and origination group
   in.read(theFileTypeVersion, 9);
   theHeaderSize+=9;
   in.read(theComplexityLevel, 2);
   theHeaderSize+=2;
   in.read(theSystemType, 4);
   theHeaderSize+=4;
   in.read(theOriginatingStationId, 10);
   theHeaderSize+=10;
   in.read(theDateTime, 14);
   theHeaderSize+=14;
   in.read(theFileTitle, 80);
   theHeaderSize+=80;
   
   // read security group
   in.read(theSecurityClassification, 1);
   theHeaderSize++;
   in.read(theCodewords, 40);
   theHeaderSize+=40;
   in.read(theControlAndHandling, 40);
   theHeaderSize+=40;
   in.read(theReleasingInstructions, 40);
   theHeaderSize+=40;
   in.read(theClassificationAuthority, 20);
   theHeaderSize+=20;
   in.read(theSecurityControlNumber, 20);
   theHeaderSize+=20;
   in.read(theSecurityDowngrade, 6);
   theHeaderSize+=6;
   if(ossimString(theSecurityDowngrade) == "999998")
   {
      in.read(theDowngradingEvent, 40);
      theHeaderSize+=40;
   }      
   in.read(theCopyNumber, 5);
   theHeaderSize+=5;
   in.read(theNumberOfCopies, 5);
   theHeaderSize+=5;
   in.read(theEncryption, 1);
   theHeaderSize++;
   in.read(theOriginatorsName, 27);
   theHeaderSize+=27;
   in.read(theOriginatorsPhone, 18);
   theHeaderSize+=18;
   
   in.read(theNitfFileLength, 12);
   theHeaderSize+=12;
   in.read(theNitfHeaderLength, 6);
   theHeaderSize+=6;
   
   // image description group
   in.read(theNumberOfImageInfoRecords, 3);
   theHeaderSize+=3;
   readImageInfoRecords(in);

   // symbol description group
   in.read(theNumberOfSymbolInfoRecords, 3);
   theHeaderSize+=3;
   readSymbolInfoRecords(in);
   
   // label description group
   in.read(theNumberOfLabelInfoRecords, 3);
   theHeaderSize+=3;
   readLabelInfoRecords(in);
   
   // text file information group
   in.read(theNumberOfTextFileInfoRecords, 3);
   theHeaderSize+=3;
   readTextFileInfoRecords(in);
   
   // Data extension group
   in.read(theNumberOfDataExtSegInfoRecords, 3);
   theHeaderSize+=3;
   readDataExtSegInfoRecords(in);
   
   // Reserve Extension Segment group
   in.read(theNumberOfResExtSegInfoRecords, 3);
   theHeaderSize+=3;
   readResExtSegInfoRecords(in);
   
   in.read(theUserDefinedHeaderDataLength, 5);
   theHeaderSize+=5;
   
   theTagList.clear();
   // only get the header overflow if there even exists
   // user defined data.
   ossim_int32 userDefinedHeaderLength = ossimString(theUserDefinedHeaderDataLength).toInt32();
   ossimNitfTagInformation         headerTag;
   
   std::streampos start   = in.tellg();
   std::streampos current = in.tellg();
   theHeaderSize+=userDefinedHeaderLength;
   if(userDefinedHeaderLength > 0)
   {
      in.read(theUserDefinedHeaderOverflow, 3);
      
      while((current - start) < userDefinedHeaderLength)
      {
         headerTag.parseStream(in);
         theTagList.push_back(headerTag);
         // in.ignore(headerTag.getTagLength());
         // headerTag.clearFields();
         current = in.tellg();
      }
   }
   in.read(theExtendedHeaderDataLength, 5);
   theHeaderSize+=5;
   ossim_int32 extendedHeaderDataLength = ossimString(theExtendedHeaderDataLength).toInt32();
   theHeaderSize+=extendedHeaderDataLength;
   
   start   = in.tellg();
   current = in.tellg();
   // for now let's just ignore it
   if(extendedHeaderDataLength > 0)
   {
      in.read(theExtendedHeaderOverflow, 3);
      
      while((current - start) < extendedHeaderDataLength)
      {
         headerTag.parseStream(in);
         theTagList.push_back(headerTag);
         in.ignore(headerTag.getTagLength());
         headerTag.clearFields();
         current = in.tellg();
      }
   }
   
   // this need to be re-thought
   initializeAllOffsets();

   if(traceDebug())
   {
      print(ossimNotify(ossimNotifyLevel_DEBUG)) << std::endl;
   }
   if(traceDebug())
   {
      ossimNotify(ossimNotifyLevel_DEBUG)<< "ossimNitfFileHeaderV2_0::parseStream:   Leaving......." << std::endl;
   }
//      initializeDisplayLevels(in);

}

void ossimNitfFileHeaderV2_0::writeStream(std::ostream &out)
{
   
}

std::ostream& ossimNitfFileHeaderV2_0::print(std::ostream& out)const
{
   out << setiosflags(std::ios::left)
       << "\nossimNitfFileHeaderV2_0::print"
       << std::setw(24) << "\nFHDR:"     << theFileTypeVersion
       << std::setw(24) << "\nCLEVEL:"   << theComplexityLevel
       << std::setw(24) << "\nSTYPE:"    << theSystemType     
       << std::setw(24) << "\nOSTAID:"   << theOriginatingStationId
       << std::setw(24) << "\nFDT:"      << theDateTime       
       << std::setw(24) << "\nFTITLE:"   << theFileTitle      
       << std::setw(24) << "\nFSCLAS:"   << theSecurityClassification
       << std::setw(24) << "\nFSCODE:"   << theCodewords
       << std::setw(24) << "\nFSCTLH:"   << theControlAndHandling
       << std::setw(24) << "\nFSREL:"    << theReleasingInstructions
       << std::setw(24) << "\nFSCAUT:"   << theClassificationAuthority
       << std::setw(24) << "\nFSCTLN:"   << theSecurityControlNumber
       << std::setw(24) << "\nFSDWNG:"   << theSecurityDowngrade
       << std::setw(24) << "\nFSDEVT:"   << theDowngradingEvent
       << std::setw(24) << "\nFSCOP:"    << theCopyNumber
       << std::setw(24) << "\nFSCPYS:"   << theNumberOfCopies
       << std::setw(24) << "\nENCRYP:"   << theEncryption
       << std::setw(24) << "\nONAME:"    << theOriginatorsName
       << std::setw(24) << "\nOPHONE:"   << theOriginatorsPhone
       << std::setw(24) << "\nFL:"       << theNitfFileLength
       << std::setw(24) << "\nHL:"       << theNitfHeaderLength
       << std::setw(24) << "\nNUMI:"     << theNumberOfImageInfoRecords;

   ossim_uint32 index;
   
   for (index = 0; index < theNitfImageInfoRecords.size(); ++index)
   {
      std::ostringstream os;
      os << std::setw(3) << std::setfill('0') << (index+1) << ":";
      
      ossimString tmpStr = "\nLISH";
      tmpStr += os.str();
      
      out << std::setw(24) << tmpStr
          << theNitfImageInfoRecords[index].theImageSubheaderLength;
      tmpStr = "\nLI";
      tmpStr += os.str();
      
      out << std::setw(24) << tmpStr
          << theNitfImageInfoRecords[index].theImageLength;
   }

   out << std::setw(24) << "\nNUMS:" << theNumberOfSymbolInfoRecords << std::endl;

   for (index = 0; index < theNitfSymbolInfoRecords.size(); ++index)
   {
      std::ostringstream os;
      os << std::setw(3) << std::setfill('0') << (index+1) << ":";

      ossimString tmpStr = "\nLSSH";
      tmpStr += os.str();

      out << tmpStr
          << theNitfSymbolInfoRecords[index].theSymbolSubheaderLength;

      tmpStr = "\nLS";
      tmpStr += os.str();

      out << tmpStr 
          << theNitfSymbolInfoRecords[index].theSymbolLength;
   }

   
   out << std::setw(24) << "\nNUML:" << theNumberOfLabelInfoRecords << std::endl;

   for (index = 0; index < theNitfLabelInfoRecords.size(); ++index)
   {
      std::ostringstream os;
      os << std::setw(3) << std::setfill('0') << (index+1) << ":";

      ossimString tmpStr = "\nLLSH";
      tmpStr += os.str();

      out << tmpStr
          << theNitfLabelInfoRecords[index].theLabelSubheaderLength;

      tmpStr = "\nLL";
      tmpStr += os.str();

      out << tmpStr 
          << theNitfLabelInfoRecords[index].theLabelLength;
   }

   out << std::setw(24) << "\nNUMT:" << theNumberOfTextFileInfoRecords << std::endl;

   for (index = 0; index < theNitfTextInfoRecords.size(); ++index)
   {
      std::ostringstream os;
      os << std::setw(3) << std::setfill('0') << (index+1) << ":";

      ossimString tmpStr = "\nLTSH";
      tmpStr += os.str();

      out << tmpStr
          << theNitfTextInfoRecords[index].theTextSubheaderLength;

      tmpStr = "\nLT";
      tmpStr += os.str();

      out << tmpStr 
          << theNitfTextInfoRecords[index].theTextLength;
   }

   out << std::setw(24) << "\nNUMDES:" << theNumberOfDataExtSegInfoRecords << std::endl;

   for (index = 0; index < theNitfDataExtSegInfoRecords.size(); ++index)
   {
      std::ostringstream os;
      os << std::setw(3) << std::setfill('0') << (index+1) << ":";

      ossimString tmpStr = "\nLDSH";
      tmpStr += os.str();

      out << tmpStr
          << theNitfDataExtSegInfoRecords[index].theDataExtSegSubheaderLength;

      tmpStr = "\nLD";
      tmpStr += os.str();

      out << tmpStr 
          << theNitfDataExtSegInfoRecords[index].theDataExtSegLength;
   }

   out << std::setw(24) << "\nNUMRES:" << theNumberOfResExtSegInfoRecords << std::endl;

   for (index = 0; index < theNitfResExtSegInfoRecords.size(); ++index)
   {
      std::ostringstream os;
      os << std::setw(3) << std::setfill('0') << (index+1) << ":";

      ossimString tmpStr = "\nLRSH";
      tmpStr += os.str();

      out << tmpStr
          << theNitfResExtSegInfoRecords[index].theResExtSegSubheaderLength;

      tmpStr = "\nLR";
      tmpStr += os.str();

      out << tmpStr 
          << theNitfResExtSegInfoRecords[index].theResExtSegLength;
   }

   out << std::setw(24) << "\nUDHDL:"   << theUserDefinedHeaderDataLength
       << std::setw(24) << "\nUDHOFL:"  << theUserDefinedHeaderOverflow
       << std::setw(24) << "\nXHDL:"    << theExtendedHeaderDataLength
       << std::endl;

   out << "TAGS:\n";
   std::copy(theTagList.begin(),
             theTagList.end(),
             std::ostream_iterator<ossimNitfTagInformation>(out,"\n"));
//   out << theHeaderTag;
   return out;
}

ossimDrect ossimNitfFileHeaderV2_0::getImageRect()const
{
   return theImageRect;
}

ossimNitfImageHeader*
ossimNitfFileHeaderV2_0::getNewImageHeader(ossim_int32 imageNumber,
                                           std::istream& in)const
{
   ossimNitfImageHeader *result = 0;
   
   if((getNumberOfImages() > 0) &&
      (imageNumber < (ossim_int32)theImageOffsetList.size()) &&
      (imageNumber >= 0))
   {
      result = allocateImageHeader();
      in.seekg(theImageOffsetList[imageNumber].theImageHeaderOffset, std::ios::beg);
      result->parseStream(in);
   }
   else
   {
      ossimNotify(ossimNotifyLevel_FATAL) << "ossimNitfFileHeaderV2_0::getNewImageHeader ERROR:"
                                          << "\nNo images in file or image number (" << imageNumber
                                          << ") is out of range!\n";
   }
   
   return result;
}

ossimNitfSymbolHeader *ossimNitfFileHeaderV2_0::getNewSymbolHeader(ossim_int32 symbolNumber,
                                                                   std::istream& in)const
{
   ossimNitfSymbolHeader *result = 0;

   if((getNumberOfSymbols() > 0) &&
      (symbolNumber < (ossim_int32)theSymbolOffsetList.size()) &&
      (symbolNumber >= 0))
   {
      result = allocateSymbolHeader();
      in.seekg(theSymbolOffsetList[symbolNumber].theSymbolHeaderOffset, std::ios::beg);
      result->parseStream(in);
   }
   
   return result;
}

ossimNitfLabelHeader *ossimNitfFileHeaderV2_0::getNewLabelHeader(ossim_int32 labelNumber,
                                                                 std::istream& in)const
{
   ossimNitfLabelHeader *result = 0;

   if((getNumberOfLabels() > 0) &&
      (labelNumber < (ossim_int32)theLabelOffsetList.size()) &&
      (labelNumber >= 0))
   {
      result = allocateLabelHeader();
      in.seekg(theLabelOffsetList[labelNumber].theLabelHeaderOffset, std::ios::beg);
      result->parseStream(in);
   }
   
   return result;
}

ossimNitfTextHeader *ossimNitfFileHeaderV2_0::getNewTextHeader(ossim_int32 textNumber,
                                                               std::istream& in)const
{
   ossimNitfTextHeader *result = 0;

   if((getNumberOfTextSegments() > 0) &&
      (textNumber < (ossim_int32)theTextOffsetList.size()) &&
      (textNumber >= 0))
   {
      result = allocateTextHeader();
      in.seekg(theTextOffsetList[textNumber].theTextHeaderOffset, std::ios::beg);
      result->parseStream(in);
   }
   
   return result;
}

ossimNitfDataExtensionSegment* ossimNitfFileHeaderV2_0::getNewDataExtensionSegment(ossim_int32 dataExtNumber,
                                                                                   std::istream& in)const
{
   ossimNitfDataExtensionSegment *result = 0;

   if((getNumberOfDataExtSegments() > 0) &&
      (dataExtNumber < (ossim_int32)theNitfDataExtSegInfoRecords.size()) &&
      (dataExtNumber >= 0))
   {
      result = allocateDataExtSegment();
      in.seekg(theDataExtSegOffsetList[dataExtNumber].theDataExtSegHeaderOffset, std::ios::beg);
      result->parseStream(in);
   }
   
   return result;
}

void ossimNitfFileHeaderV2_0::initializeDisplayLevels(std::istream& in)
{
   // we will need to temporarily save the get pointer since
   // initializeDisplayLevels changes it.
   std::streampos saveTheGetPointer = in.tellg();
   
   std::vector<ossimNitfImageOffsetInformation>::iterator imageOffsetList = theImageOffsetList.begin();

   // allocate temporary space to store image headers
   ossimNitfImageHeader* imageHeader = allocateImageHeader();

   // clear the list
   theDisplayInformationList.clear();
   
   theImageRect = ossimDrect(0,0,0,0);
   if(!imageHeader)
   {
      return;
   }
   
   ossim_uint32 idx = 0;
   while(imageOffsetList != theImageOffsetList.end())
   {
      // position the get pointer in the input
      // stream to the start of the image header
      in.seekg((*imageOffsetList).theImageHeaderOffset, std::ios::beg);
      // get the data
      imageHeader->parseStream(in);
      // create a union of rects.  The result should be the image rect.
      ossimDrect tempRect = imageHeader->getImageRect();
      if((tempRect.width() > 1) &&
         (tempRect.height() > 1))
      {
         theImageRect = theImageRect.combine(tempRect);
      }
      
      insertIntoDisplayInfoList(ossimNitfDisplayInfo(ossimString("IM"),
                                                     imageHeader->getDisplayLevel(),
                                                     idx));
      
      ++imageOffsetList;
      ++idx;                                       
   }
   delete imageHeader;
   imageHeader = 0;


   
   // finally we reset the saved get state back
   // to its original position
   in.seekg(saveTheGetPointer, std::ios::beg);
}

void ossimNitfFileHeaderV2_0::insertIntoDisplayInfoList(const ossimNitfDisplayInfo &displayInformation)
{
   std::vector<ossimNitfDisplayInfo>::iterator displayList = theDisplayInformationList.begin();

   while(displayList != theDisplayInformationList.end())
   {
      if(displayInformation.theDisplayLevel < (*displayList).theDisplayLevel)
      {
         theDisplayInformationList.insert(displayList, displayInformation);
         return;
      }
      ++displayList;
   }

   // If we get here it means it's larger than all others
   // and we push onto the end
   theDisplayInformationList.push_back(displayInformation);
}

void ossimNitfFileHeaderV2_0::initializeAllOffsets()
{
   // this will be a running tally 
   ossim_uint64 tally = theHeaderSize;
   ossim_uint32 idx = 0;

   // clear out all offset inforamtion and begin populating them
   theImageOffsetList.clear();
   theSymbolOffsetList.clear();
   theLabelOffsetList.clear();
   theLabelOffsetList.clear();
   

   for(idx = 0; idx < (ossim_int32)theNitfImageInfoRecords.size(); idx++)
   {
      theImageOffsetList.push_back(ossimNitfImageOffsetInformation(tally,
                                                                   tally + theNitfImageInfoRecords[idx].getHeaderLength()));
      tally += theNitfImageInfoRecords[idx].getTotalLength();
   }
   for(idx = 0; idx < (ossim_int32)theNitfSymbolInfoRecords.size(); idx++)
   {
      theSymbolOffsetList.push_back(ossimNitfSymbolOffsetInformation(tally,
                                                                     tally + theNitfSymbolInfoRecords[idx].getHeaderLength()));
      tally += theNitfSymbolInfoRecords[idx].getTotalLength();
   }

   for(idx = 0; idx < (ossim_int32)theNitfLabelInfoRecords.size(); idx++)
   {
      theLabelOffsetList.push_back(ossimNitfLabelOffsetInformation(tally,
                                                                   tally + theNitfLabelInfoRecords[idx].getHeaderLength()));
      tally += theNitfLabelInfoRecords[idx].getTotalLength();
   }

   for(idx = 0; idx < (ossim_int32)theNitfTextInfoRecords.size(); idx++)
   {
      theTextOffsetList.push_back(ossimNitfTextOffsetInformation(tally,
                                                                 tally + theNitfTextInfoRecords[idx].getHeaderLength()));
      tally += theNitfTextInfoRecords[idx].getTotalLength();
   }

   for(idx = 0; idx < (ossim_int32)theNitfDataExtSegInfoRecords.size(); idx++)
   {
      theDataExtSegOffsetList.push_back(ossimNitfDataExtSegOffsetInformation(tally,
                                                                             tally + theNitfDataExtSegInfoRecords[idx].getHeaderLength()));
      tally += theNitfDataExtSegInfoRecords[idx].getTotalLength();
   }
}

ossimNitfImageHeader *ossimNitfFileHeaderV2_0::allocateImageHeader()const
{
   return new ossimNitfImageHeaderV2_0;
}

ossimNitfSymbolHeader *ossimNitfFileHeaderV2_0::allocateSymbolHeader()const
{
   return new ossimNitfSymbolHeaderV2_0;
}

ossimNitfLabelHeader *ossimNitfFileHeaderV2_0::allocateLabelHeader()const
{
   return new ossimNitfLabelHeaderV2_0;
}

ossimNitfTextHeader *ossimNitfFileHeaderV2_0::allocateTextHeader()const
{
   return new ossimNitfTextHeaderV2_0;
}

ossimNitfDataExtensionSegment* ossimNitfFileHeaderV2_0::allocateDataExtSegment()const
{
   return new ossimNitfDataExtensionSegmentV2_0;
}

bool ossimNitfFileHeaderV2_0::isEncrypted()const
{
   return (theEncryption[0] == '1');
}

ossim_int32 ossimNitfFileHeaderV2_0::getNumberOfImages()const
{
   return theNitfImageInfoRecords.size();
}

ossim_int32 ossimNitfFileHeaderV2_0::getNumberOfLabels()const
{
   return (theNitfLabelInfoRecords.size());
}

ossim_int32 ossimNitfFileHeaderV2_0::getNumberOfSymbols()const
{
   return (theNitfSymbolInfoRecords.size());
}

ossim_int32 ossimNitfFileHeaderV2_0::getNumberOfGraphics()const
{
   return 0;
}

ossim_int32 ossimNitfFileHeaderV2_0::getNumberOfTextSegments()const
{
   return theNitfTextInfoRecords.size();
}

ossim_int32 ossimNitfFileHeaderV2_0::getNumberOfDataExtSegments()const
{
   return theNitfDataExtSegInfoRecords.size();
}

ossim_int32 ossimNitfFileHeaderV2_0::getHeaderSize()const
{
   return theHeaderSize;
}

ossim_int32 ossimNitfFileHeaderV2_0::getFileSize()const
{
   ossimString temp = theNitfFileLength;
   if(temp == "999999999999")
   {
      return -1;
   }
   else
   {
      return temp.toInt32();
   }
}

const char* ossimNitfFileHeaderV2_0::getVersion()const
{
   return &theFileTypeVersion[4];
}

const char* ossimNitfFileHeaderV2_0::getDateTime()const
{
   return theDateTime;
}

ossimString ossimNitfFileHeaderV2_0::getSecurityClassification()const
{
   return theSecurityClassification;
}

void ossimNitfFileHeaderV2_0::clearFields()
{
   theDisplayInformationList.clear();
   theImageOffsetList.clear();
   theSymbolOffsetList.clear();
   theLabelOffsetList.clear();
   theTextOffsetList.clear();
   theDataExtSegOffsetList.clear();
   theNitfSymbolInfoRecords.clear();
   theNitfLabelInfoRecords.clear();
   theNitfTextInfoRecords.clear();
   theNitfDataExtSegInfoRecords.clear();
   theNitfResExtSegInfoRecords.clear();
   
   theFilename = "";
   memset(theFileTypeVersion, ' ', 9);
   memset(theComplexityLevel, ' ', 2);
   memset(theSystemType, ' ', 4);
   memset(theOriginatingStationId, ' ', 10);
   memset(theDateTime, ' ', 14);
   memset(theFileTitle, ' ', 80);
   memset(theSecurityClassification, ' ', 1);
   memset(theCodewords, ' ', 40);
   memset(theControlAndHandling, ' ', 40);
   memset(theReleasingInstructions, ' ', 40);
   memset(theClassificationAuthority, ' ', 20);
   memset(theSecurityControlNumber, ' ', 20);
   memset(theSecurityDowngrade, ' ', 6);
   memset(theDowngradingEvent, ' ', 40);
   memset(theCopyNumber, ' ', 5);
   memset(theNumberOfCopies, ' ', 5);
   memset(theEncryption, ' ', 1);
   memset(theOriginatorsName, ' ', 27);
   memset(theOriginatorsPhone, ' ', 18);
   memset(theNitfFileLength, ' ', 12);
   memset(theNitfHeaderLength, ' ', 6);
   memset(theNumberOfImageInfoRecords, ' ', 3);
   memset(theNumberOfSymbolInfoRecords, ' ', 3);
   memset(theNumberOfLabelInfoRecords, ' ', 3);
   memset(theNumberOfTextFileInfoRecords, ' ', 3);
   memset(theNumberOfDataExtSegInfoRecords, ' ', 3);
   memset(theNumberOfResExtSegInfoRecords, ' ', 3);
   memset(theUserDefinedHeaderDataLength, ' ', 5);
   memset(theUserDefinedHeaderOverflow, ' ', 3);
   memset(theExtendedHeaderDataLength, ' ', 5);
   memset(theExtendedHeaderOverflow, ' ', 3);
   
   theFileTypeVersion[9] = '\0';
   theComplexityLevel[2] = '\0';
   theSystemType[4]      = '\0';
   theOriginatingStationId[10] = '\0';
   theDateTime[14]       = '\0';
   theFileTitle[80]      = '\0';
   theSecurityClassification[1] = '\0';
   theCodewords[40] = '\0';
   theControlAndHandling[40] = '\0';
   theReleasingInstructions[40] = '\0';
   theClassificationAuthority[20] = '\0';
   theSecurityControlNumber[20] = '\0';
   theSecurityDowngrade[6] = '\0';
   theDowngradingEvent[40] = '\0';
   theCopyNumber[5] = '\0';
   theNumberOfCopies[5] = '\0';
   theEncryption[1] = '\0';
   theOriginatorsName[27] = '\0';
   theOriginatorsPhone[18] = '\0';
   theNitfFileLength[12]  = '\0';
   theNitfHeaderLength[6] = '\0';
   theNumberOfImageInfoRecords[3] = '\0';
   theNumberOfSymbolInfoRecords[3] = '\0';
   theNumberOfLabelInfoRecords[3] = '\0';
   theNumberOfTextFileInfoRecords[3] = '\0';
   theNumberOfDataExtSegInfoRecords[3] = '\0';
   theNumberOfResExtSegInfoRecords[3] = '\0';
   theUserDefinedHeaderDataLength[5] = '\0';
   theUserDefinedHeaderOverflow[3] = '\0';
   theExtendedHeaderDataLength[5] = '\0';
   theExtendedHeaderOverflow[3] = '\0';
   theHeaderSize = 0;
}

void ossimNitfFileHeaderV2_0::readImageInfoRecords(std::istream &in)
{
   ossim_int32 numberOfImages = ossimString(theNumberOfImageInfoRecords).toInt32();
   ossim_int32 index;

   theNitfImageInfoRecords.clear();
   for(index=0; index < numberOfImages; index++)
   {
      ossimNitfImageInfoRecordV2_0 temp;
      
      in.read(temp.theImageSubheaderLength, 6);
      in.read(temp.theImageLength, 10);
      theHeaderSize+=16;
      temp.theImageSubheaderLength[6] = '\0';
      temp.theImageLength[10] = '\0';

      theNitfImageInfoRecords.push_back(temp);
   }
}

void ossimNitfFileHeaderV2_0::readSymbolInfoRecords(std::istream &in)
{
   ossim_int32 numberOfSymbols = ossimString(theNumberOfSymbolInfoRecords).toInt32();
   ossim_int32 index;

   theNitfSymbolInfoRecords.clear();
   
   for(index=0; index < numberOfSymbols; index++)
   {
      ossimNitfSymbolInfoRecordV2_0 temp;

      
      in.read(temp.theSymbolSubheaderLength, 4);
      in.read(temp.theSymbolLength, 6);
      theHeaderSize+=10;
      
      temp.theSymbolSubheaderLength[4] = '\0';
      temp.theSymbolLength[6] = '\0';
      
      theNitfSymbolInfoRecords.push_back(temp);
   }
}

void ossimNitfFileHeaderV2_0::readLabelInfoRecords(std::istream &in)
{
   ossim_int32 numberOfLabels = ossimString(theNumberOfLabelInfoRecords).toInt32();
   ossim_int32 index;

   theNitfLabelInfoRecords.clear();
   
   for(index=0; index < numberOfLabels; index++)
   {
      ossimNitfLabelInfoRecordV2_0 temp;
      
      in.read(temp.theLabelSubheaderLength, 4);
      in.read(temp.theLabelLength, 3);
      theHeaderSize+=7;
      temp.theLabelSubheaderLength[4] = '\0';
      temp.theLabelLength[3]          = '\0';

      theNitfLabelInfoRecords.push_back(temp);
   }
}

void ossimNitfFileHeaderV2_0::readTextFileInfoRecords(std::istream &in)
{
   ossim_int32 numberOfTextFiles = ossimString(theNumberOfTextFileInfoRecords).toInt32();
   ossim_int32 index;

   theNitfTextInfoRecords.clear();
   for(index=0; index < numberOfTextFiles; index++)
   {
      ossimNitfTextInfoRecordV2_0 temp;
      
      in.read(temp.theTextSubheaderLength, 4);
      in.read(temp.theTextLength, 5);
      theHeaderSize+=9;
      
      temp.theTextSubheaderLength[4] = '\0';
      temp.theTextLength[5] = '\0';
      
      theNitfTextInfoRecords.push_back(temp);
   }
}

void ossimNitfFileHeaderV2_0::readDataExtSegInfoRecords(std::istream &in)
{
   ossim_int32 numberOfDataExtSegs = ossimString(theNumberOfDataExtSegInfoRecords).toInt32();
   ossim_int32 index;

   theNitfDataExtSegInfoRecords.clear();
   for(index=0; index < numberOfDataExtSegs; index++)
   {
      ossimNitfDataExtSegInfoRecordV2_0 temp;
      
      in.read(temp.theDataExtSegSubheaderLength, 4);
      in.read(temp.theDataExtSegLength, 9);
      theHeaderSize+=13;
      
      temp.theDataExtSegSubheaderLength[4] = '\0';
      temp.theDataExtSegLength[9]          = '\0';

      theNitfDataExtSegInfoRecords.push_back(temp);
   }
}

void ossimNitfFileHeaderV2_0::readResExtSegInfoRecords(std::istream &in)
{
   ossim_int32 numberOfResExtSegs = ossimString(theNumberOfResExtSegInfoRecords).toInt32();
   ossim_int32 index;

   theNitfResExtSegInfoRecords.clear();
   for(index=0; index < numberOfResExtSegs; index++)
   {
      ossimNitfResExtSegInfoRecordV2_0 temp;

      in.read(temp.theResExtSegSubheaderLength, 4);
      in.read(temp.theResExtSegLength, 7);
      theHeaderSize+=11;
      
      temp.theResExtSegSubheaderLength[4] = '\0';
      temp.theResExtSegLength[7]          = '\0';
      
      theNitfResExtSegInfoRecords.push_back(temp);      
   }
}

void ossimNitfFileHeaderV2_0::setComplianceLevel(const ossimString& complianceLevel)
{
   ossimNitfCommon::setField(theComplexityLevel, complianceLevel, 2);
}

void ossimNitfFileHeaderV2_0::setCodeWords(const ossimString& codeWords)
{
   ossimNitfCommon::setField(theCodewords, codeWords, 40);
}

void ossimNitfFileHeaderV2_0::setControlAndHandling(const ossimString& controlAndHandling)
{
   ossimNitfCommon::setField(theControlAndHandling, controlAndHandling, 40);
}

void ossimNitfFileHeaderV2_0::setReleasingInstructions(const ossimString& releasingInstructions)
{
   ossimNitfCommon::setField(theReleasingInstructions, releasingInstructions, 40);
}

void ossimNitfFileHeaderV2_0::setClassificationAuthority(const ossimString& classAuth)
{
   ossimNitfCommon::setField(theClassificationAuthority, classAuth, 20);
}

void ossimNitfFileHeaderV2_0::setSecurityControlNumber(const ossimString& controlNo)
{
   ossimNitfCommon::setField(theSecurityControlNumber, controlNo, 20);
}

void ossimNitfFileHeaderV2_0::setOriginatorsName(const ossimString& originatorName)
{
   ossimNitfCommon::setField(theOriginatorsName, originatorName, 27);
}

void ossimNitfFileHeaderV2_0::setOriginatorsPhone(const ossimString& originatorPhone)
{
   ossimNitfCommon::setField(theOriginatorsPhone, originatorPhone, 18);
}

void ossimNitfFileHeaderV2_0::setSecurityDowngrade(const ossimString& securityDowngrade)
{
   ossimNitfCommon::setField(theSecurityDowngrade, securityDowngrade, 6);
}

void ossimNitfFileHeaderV2_0::setDowngradingEvent(const ossimString& downgradeEvent)
{
   ossimNitfCommon::setField(theDowngradingEvent, downgradeEvent, 40);
}

ossimString ossimNitfFileHeaderV2_0::getComplianceLevel()const
{
   return theComplexityLevel;
}

ossimString ossimNitfFileHeaderV2_0::getSecurityDowngrade()const
{
   return theSecurityDowngrade;
}

ossimString ossimNitfFileHeaderV2_0::getDowngradingEvent()const
{
   return theDowngradingEvent;
}

ossimString ossimNitfFileHeaderV2_0::getCodeWords()const
{
   return theCodewords;
}

ossimString ossimNitfFileHeaderV2_0::getControlAndHandling()const
{
   return theControlAndHandling;
}

ossimString ossimNitfFileHeaderV2_0::getReleasingInstructions()const
{
   return theReleasingInstructions;
}

ossimString ossimNitfFileHeaderV2_0::getClassificationAuthority()const
{
   return theClassificationAuthority;
}

ossimString ossimNitfFileHeaderV2_0::getSecurityControlNumber()const
{
   return theSecurityControlNumber;
}

ossimString ossimNitfFileHeaderV2_0::getOriginatorsName()const
{
   return theOriginatorsName;
}

ossimString ossimNitfFileHeaderV2_0::getOriginatorsPhone()const
{
   return theOriginatorsPhone;
}

void ossimNitfFileHeaderV2_0::setProperty(ossimRefPtr<ossimProperty> property)
{
   const ossimString& name = property->getName();
   if(name == CLEVEL_KW)
   {
      setComplianceLevel(property->valueToString());
   }
   else if(name == FSDWNG_KW)
   {
      setSecurityDowngrade(property->valueToString());
   }
   else if(name == FSDEVT_KW)
   {
      setDowngradingEvent(property->valueToString());
   }
   else if(name == ONAME_KW)
   {
      setOriginatorsName(property->valueToString());
   }
   else if(name == OPHONE_KW)
   {
      setOriginatorsPhone(property->valueToString());
   }
   else
   {
      ossimNitfFileHeaderV2_X::setProperty(property);
   }
}

ossimRefPtr<ossimProperty> ossimNitfFileHeaderV2_0::getProperty(const ossimString& name)const
{
   ossimProperty* property = NULL;

   if(name == CLEVEL_KW)
   {
      property = new ossimStringProperty(name, ossimString(theComplexityLevel).trim());
   }
   else if(name == FSDWNG_KW)
   {
      property = new ossimStringProperty(name, ossimString(theSecurityDowngrade).trim());
   }
   else if(name == FSDEVT_KW)
   {
      property = new ossimStringProperty(name, ossimString(theDowngradingEvent).trim());
   }
   else if(name == ONAME_KW)
   {
      property = new ossimStringProperty(name, ossimString(theOriginatorsName).trim());
   }
   else if(name == OPHONE_KW)
   {
      property = new ossimStringProperty(name, ossimString(theOriginatorsPhone).trim());
   }
   else
   {
      return ossimNitfFileHeader::getProperty(name);
   }
   return property;
}

void ossimNitfFileHeaderV2_0::getPropertyNames(std::vector<ossimString>& propertyNames)const
{
   ossimNitfFileHeader::getPropertyNames(propertyNames);
   propertyNames.push_back(CLEVEL_KW);
   propertyNames.push_back(STYPE_KW);
   propertyNames.push_back(OSTAID_KW);
   propertyNames.push_back(FDT_KW);
   propertyNames.push_back(FTITLE_KW);
   propertyNames.push_back(FSCLAS_KW);
   propertyNames.push_back(FSCODE_KW);
   propertyNames.push_back(FSCTLH_KW);
   propertyNames.push_back(FSREL_KW);
   propertyNames.push_back(FSCAUT_KW);
   propertyNames.push_back(FSCTLN_KW);
   propertyNames.push_back(FSCOP_KW);
   propertyNames.push_back(FSCPYS_KW);
   propertyNames.push_back(ENCRYP_KW);
   propertyNames.push_back(ONAME_KW);
   propertyNames.push_back(OPHONE_KW);
}
