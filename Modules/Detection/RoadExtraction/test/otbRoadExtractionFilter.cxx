/*
 * Copyright (C) 2005-2017 Centre National d'Etudes Spatiales (CNES)
 *
 * This file is part of Orfeo Toolbox
 *
 *     https://www.orfeo-toolbox.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "itkMacro.h"

#include "otbImage.h"
#include "otbRoadExtractionFilter.h"
#include "otbDrawPathListFilter.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "otbPolyLineParametricPathWithValue.h"
#include "otbMath.h"

int otbRoadExtractionFilter(int itkNotUsed(argc), char * argv[])
{
  const unsigned int Dimension = 2;
  typedef otb::VectorImage<double, Dimension>                     InputImageType;
  typedef otb::Image<double, Dimension>                           OutputImageType;
  typedef otb::PolyLineParametricPathWithValue<double, Dimension> PathType;

  typedef otb::ImageFileReader<InputImageType>                                ReaderType;
  typedef otb::ImageFileWriter<OutputImageType>                               WriterType;
  typedef otb::RoadExtractionFilter<InputImageType, PathType>                 RoadExtractionFilterType;
  typedef RoadExtractionFilterType::InputPixelType                            InputPixelType;
  typedef otb::DrawPathListFilter<OutputImageType, PathType, OutputImageType> DrawPathFilterType;

  //Parameters
  const char *   inputFileName(argv[1]);
  const char *   outputFileName(argv[2]);
  InputPixelType ReferencePixel;
  ReferencePixel.Reserve(4);
  ReferencePixel.SetElement(0, ::atof(argv[3]));
  ReferencePixel.SetElement(1, ::atof(argv[4]));
  ReferencePixel.SetElement(2, ::atof(argv[5]));
  ReferencePixel.SetElement(3, ::atof(argv[6]));
  const double Alpha = ::atof(argv[7]);
  const double AmplitudeThreshold = ::atof(argv[8]);
  const double Tolerance = ::atof(argv[9]);
  const double MaxAngle = ((otb::CONST_PI * ::atof(argv[10])) / 180.);
  const double FirstMeanDistanceThreshold  = ::atof(argv[11]);
  const double SecondMeanDistanceThreshold  = ::atof(argv[12]);
  const double LinkAngularThreshold = ((otb::CONST_PI * ::atof(argv[13])) / 180.);
  const double LinkDistanceThreshold  = ::atof(argv[14]);

  // Instantiating object
  ReaderType::Pointer               reader = ReaderType::New();
  RoadExtractionFilterType::Pointer roadExtraction = RoadExtractionFilterType::New();
  DrawPathFilterType::Pointer       draw = DrawPathFilterType::New();
  WriterType::Pointer               writer = WriterType::New();

  //Initialization parameters
  reader->SetFileName(inputFileName);

  roadExtraction->SetInput(reader->GetOutput());
  roadExtraction->SetReferencePixel(ReferencePixel);
  roadExtraction->SetAlpha(Alpha);
  roadExtraction->SetAmplitudeThreshold(AmplitudeThreshold);
  roadExtraction->SetTolerance(Tolerance);
  roadExtraction->SetMaxAngle(MaxAngle);
  roadExtraction->SetFirstMeanDistanceThreshold(FirstMeanDistanceThreshold);
  roadExtraction->SetSecondMeanDistanceThreshold(SecondMeanDistanceThreshold);
  roadExtraction->SetAngularThreshold(LinkAngularThreshold);
  roadExtraction->SetDistanceThreshold(LinkDistanceThreshold);

  reader->GenerateOutputInformation();
  OutputImageType::Pointer image = OutputImageType::New();
  image->CopyInformation(reader->GetOutput());
  image->SetRegions(image->GetLargestPossibleRegion());
  image->Allocate();
  image->FillBuffer(0);

  draw->SetInput(image);
  //Use internal value of path
  draw->UseInternalPathValueOn();
  draw->SetInputPath(roadExtraction->GetOutput());

  writer->SetFileName(outputFileName);
  writer->SetInput(draw->GetOutput());
  writer->Update();

  return EXIT_SUCCESS;
}
