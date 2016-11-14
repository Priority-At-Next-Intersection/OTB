/*=========================================================================
 Program:   ORFEO Toolbox
 Language:  C++
 Date:      $Date$
 Version:   $Revision$


 Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
 See OTBCopyright.txt for details.


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/
#include "otbWrapperCompositeApplication.h"
#include "otbWrapperApplicationFactory.h"

#include "otbOGRDataToSamplePositionFilter.h"
#include "otbSamplingRateCalculator.h"

namespace otb
{
namespace Wrapper
{

class TrainImagesClassifier: public CompositeApplication
{
public:
  /** Standard class typedefs. */
  typedef TrainImagesClassifier Self;
  typedef CompositeApplication Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard macro */
  itkNewMacro(Self)

  itkTypeMacro(TrainImagesClassifier, otb::Wrapper::CompositeApplication)

  /** filters typedefs*/
  typedef otb::OGRDataToSamplePositionFilter<
    FloatVectorImageType,
    UInt8ImageType,
    otb::PeriodicSampler>                           PeriodicSamplerType;

  typedef otb::SamplingRateCalculator::MapRateType  MapRateType;

protected:

private:

void DoInit() ITK_OVERRIDE
{
  SetName("TrainImagesClassifier");
  SetDescription(
    "Train a classifier from multiple pairs of images and training vector data.");

  // Documentation
  SetDocName("Train a classifier from multiple images");
  SetDocLongDescription(
    "This application performs a classifier training from multiple pairs of input images and training vector data. "
    "Samples are composed of pixel values in each band optionally centered and reduced using an XML statistics file produced by "
    "the ComputeImagesStatistics application.\n The training vector data must contain polygons with a positive integer field "
    "representing the class label. The name of this field can be set using the \"Class label field\" parameter. Training and validation "
    "sample lists are built such that each class is equally represented in both lists. One parameter allows controlling the ratio "
    "between the number of samples in training and validation sets. Two parameters allow managing the size of the training and "
    "validation sets per class and per image.\n Several classifier parameters can be set depending on the chosen classifier. In the "
    "validation process, the confusion matrix is organized the following way: rows = reference labels, columns = produced labels. "
    "In the header of the optional confusion matrix output file, the validation (reference) and predicted (produced) class labels"
    " are ordered according to the rows/columns of the confusion matrix.\n This application is based on LibSVM and OpenCV Machine Learning "
    "(2.3.1 and later).");
  SetDocLimitations("None");
  SetDocAuthors("OTB-Team");
  SetDocSeeAlso("OpenCV documentation for machine learning http://docs.opencv.org/modules/ml/doc/ml.html ");

  AddApplication("PolygonClassStatistics", "polystat","Polygon analysis");
  AddApplication("MultiImageSamplingRate", "rates", "Sampling rates");
  AddApplication("SampleSelection", "select", "Sample selection");
  AddApplication("SampleExtraction","extraction", "Sample extraction");
  AddApplication("TrainVectorClassifier", "training", "Model training");

  //Group IO
  AddParameter(ParameterType_Group, "io", "Input and output data");
  SetParameterDescription("io", "This group of parameters allows setting input and output data.");

  AddParameter(ParameterType_InputImageList, "io.il", "Input Image List");
  SetParameterDescription("io.il", "A list of input images.");
  AddParameter(ParameterType_InputVectorDataList, "io.vd", "Input Vector Data List");
  SetParameterDescription("io.vd", "A list of vector data to select the training samples.");

  ShareParameter("io.imstat","training.io.stats");
  ShareParameter("io.confmatout","training.io.confmatout");
  ShareParameter("io.out","training.io.out");

  // TODO : handle elev parameters ?

  // Sampling settings
  AddParameter(ParameterType_Group, "sample", "Training and validation samples parameters");
  SetParameterDescription("sample",
    "This group of parameters allows you to set training and validation sample lists parameters.");
  AddParameter(ParameterType_Int, "sample.mt", "Maximum training sample size per class");
  SetDefaultParameterInt("sample.mt", 1000);
  SetParameterDescription("sample.mt", "Maximum size per class (in pixels) of "
    "the training sample list (default = 1000) (no limit = -1). If equal to -1,"
    " then the maximal size of the available training sample list per class "
    "will be equal to the surface area of the smallest class multiplied by the"
    " training sample ratio.");
  AddParameter(ParameterType_Int, "sample.mv", "Maximum validation sample size per class");
  SetDefaultParameterInt("sample.mv", 1000);
  SetParameterDescription("sample.mv", "Maximum size per class (in pixels) of "
    "the validation sample list (default = 1000) (no limit = -1). If equal to -1,"
    " then the maximal size of the available validation sample list per class "
    "will be equal to the surface area of the smallest class multiplied by the "
    "validation sample ratio.");
  AddParameter(ParameterType_Int, "sample.bm", "Bound sample number by minimum");
  SetDefaultParameterInt("sample.bm", 1);
  SetParameterDescription("sample.bm", "Bound the number of samples for each "
    "class by the number of available samples by the smaller class. Proportions "
    "between training and validation are respected. Default is true (=1).");
  AddParameter(ParameterType_Float, "sample.vtr", "Training and validation sample ratio");
  SetParameterDescription("sample.vtr",
    "Ratio between training and validation samples (0.0 = all training, 1.0 = "
    "all validation) (default = 0.5).");
  SetParameterFloat("sample.vtr", 0.5);
  SetMaximumParameterFloatValue("sample.vtr",1.0);
  SetMinimumParameterFloatValue("sample.vtr",0.0);

  ShareParameter("sample.vfn","training.cfield");

  // hide sampling parameters
  //ShareParameter("sample.strategy","rates.strategy");
  //ShareParameter("sample.mim","rates.mim");

  // Classifier settings
  ShareParameter("classifier","training.classifier");

  ShareParameter("rand","training.rand");

  // Synchronization between applications
  Connect("select.field", "polystat.field");
  Connect("select.layer", "polystat.layer");

  Connect("extraction.in",    "select.in");
  Connect("extraction.vec",   "select.out");
  Connect("extraction.field", "select.field");
  Connect("extraction.layer", "select.layer");

  ShareParameter("ram","polystat.ram");
  Connect("select.ram", "polystat.ram");
  Connect("extraction.ram", "polystat.ram");

  Connect("select.rand", "training.rand");

  // Doc example parameter settings
  SetDocExampleParameterValue("io.il", "QB_1_ortho.tif");
  SetDocExampleParameterValue("io.vd", "VectorData_QB1.shp");
  SetDocExampleParameterValue("io.imstat", "EstimateImageStatisticsQB1.xml");
  SetDocExampleParameterValue("sample.mv", "100");
  SetDocExampleParameterValue("sample.mt", "100");
  SetDocExampleParameterValue("sample.vtr", "0.5");
  SetDocExampleParameterValue("sample.edg", "false");
  SetDocExampleParameterValue("sample.vfn", "Class");
  SetDocExampleParameterValue("classifier", "libsvm");
  SetDocExampleParameterValue("classifier.libsvm.k", "linear");
  SetDocExampleParameterValue("classifier.libsvm.c", "1");
  SetDocExampleParameterValue("classifier.libsvm.opt", "false");
  SetDocExampleParameterValue("io.out", "svmModelQB1.txt");
  SetDocExampleParameterValue("io.confmatout", "svmConfusionMatrixQB1.csv");
}

void DoUpdateParameters() ITK_OVERRIDE
{
}

void DoExecute() ITK_OVERRIDE
{
  FloatVectorImageListType* imageList = GetParameterImageList("io.il");
  std::vector<std::string> vectorFileList = GetParameterStringList("io.vd");
  unsigned int nbInputs = imageList->Size();

  // Prepare temporary file names
  std::string outModel(GetParameterString("io.out"));
  std::vector<std::string> polyStatOutputs;
  std::vector<std::string> ratesOutputs;
  std::vector<std::string> sampleOutputs;
  std::vector<std::string> sampleTrainOutputs;
  std::vector<std::string> sampleValidOutputs;
  std::string rateOut(outModel + "_rates.csv");
  for (unsigned int i=0 ; i<nbInputs ; i++)
    {
    std::ostringstream oss;
    oss <<i+1;
    std::string strIndex(oss.str());
    polyStatOutputs.push_back(outModel + "_stats_" + strIndex + ".xml");
    ratesOutputs.push_back(outModel + "_rates_" + strIndex + ".csv");
    sampleOutputs.push_back(outModel + "_samples_" + strIndex + ".sqlite");
    sampleTrainOutputs.push_back(outModel + "_trainSamples_" + strIndex + ".sqlite");
    sampleValidOutputs.push_back(outModel + "_validSamples_" + strIndex + ".sqlite");
    }

  // Polygons stats
  for (unsigned int i=0 ; i<nbInputs ; i++)
    {
    GetInternalApplication("polystat")->SetParameterInputImage("in",imageList->GetNthElement(i));
    GetInternalApplication("polystat")->SetParameterString("vec",vectorFileList[i]);
    GetInternalApplication("polystat")->SetParameterString("out",polyStatOutputs[i]);
    ExecuteInternal("polystat");
    }

  // compute sampling rates
  GetInternalApplication("rates")->SetParameterStringList("il",polyStatOutputs);
  GetInternalApplication("rates")->SetParameterString("out",rateOut);
  GetInternalApplication("rates")->SetParameterString("mim","proportional");
  double vtr = GetParameterFloat("sample.vtr");
  if (GetParameterInt("sample.bm") != 0)
    {
    GetInternalApplication("rates")->SetParameterString("strategy","smallest");
    }
  else
    {
    long mt = GetParameterInt("sample.mt");
    long mv = GetParameterInt("sample.mv");
    if (mt > -1)
      {
      if (mv > -1)
        {
        GetInternalApplication("rates")->SetParameterString("strategy","constant");
        GetInternalApplication("rates")->SetParameterInt("strategy.constant.nb",mt+mv);
        }
      else
        {
        if (vtr < 0.99999)
          {
          GetInternalApplication("rates")->SetParameterString("strategy","constant");
          GetInternalApplication("rates")->SetParameterInt("strategy.constant.nb", static_cast<int>((double) mt / (1.0 - vtr)));
          }
        else
          {
          // no limit on validation samples and almost no training : take all samples
          GetInternalApplication("rates")->SetParameterString("strategy","all");
          }
        }
      }
    else
      {
      if (mv > -1)
        {
        if (vtr > 0.00001)
          {
          GetInternalApplication("rates")->SetParameterString("strategy","constant");
          GetInternalApplication("rates")->SetParameterInt("strategy.constant.nb", static_cast<int>((double) mv / vtr));
          }
        else
          {
          // no limit on training samples and almost no validation samples : take all samples
          GetInternalApplication("rates")->SetParameterString("strategy","all");
          }
        }
      else
        {
        GetInternalApplication("rates")->SetParameterString("strategy","all");
        }
      }
    }
  ExecuteInternal("rates");

  // Select & extract samples
  GetInternalApplication("select")->SetParameterString("sampler", "random");
  GetInternalApplication("select")->SetParameterString("strategy","byclass");
  GetInternalApplication("extraction")->SetParameterString("outfield", "prefix");
  GetInternalApplication("extraction")->SetParameterString("outfield.prefix.name","value_");
  for (unsigned int i=0 ; i<nbInputs ; i++)
    {
    GetInternalApplication("select")->SetParameterInputImage("in",imageList->GetNthElement(i));
    GetInternalApplication("select")->SetParameterString("vec",vectorFileList[i]);
    GetInternalApplication("select")->SetParameterString("out",sampleOutputs[i]);
    GetInternalApplication("select")->SetParameterString("instats",polyStatOutputs[i]);
    GetInternalApplication("select")->SetParameterString("strategy.byclass.in",ratesOutputs[i]);
    // select sample positions
    ExecuteInternal("select");
    // extract sample descriptors
    ExecuteInternal("extraction");

    // Split between training and validation
    ogr::DataSource::Pointer source = ogr::DataSource::New(sampleOutputs[i], ogr::DataSource::Modes::Read);
    ogr::DataSource::Pointer destTrain = ogr::DataSource::New(sampleTrainOutputs[i], ogr::DataSource::Modes::Overwrite);
    ogr::DataSource::Pointer destValid = ogr::DataSource::New(sampleValidOutputs[i], ogr::DataSource::Modes::Overwrite);
    // read sampling rates from ratesOutputs[i]
    SamplingRateCalculator::Pointer rateCalculator = SamplingRateCalculator::New();
    rateCalculator->Read(ratesOutputs[i]);
    // Compute sampling rates for train and valid
    const MapRateType &inputRates = rateCalculator->GetRatesByClass();
    MapRateType trainRates;
    MapRateType validRates;
    otb::SamplingRateCalculator::TripletType tpt;
    for (MapRateType::const_iterator it = inputRates.begin() ;
         it != inputRates.end() ;
         ++it)
      {
      unsigned long total = std::min(it->second.Required,it->second.Tot );
      unsigned long neededValid = static_cast<unsigned long>((double) total * vtr );
      unsigned long neededTrain = total - neededValid;
      tpt.Tot = total;
      tpt.Required = neededTrain;
      tpt.Rate = (1.0 - vtr);
      trainRates[it->first] = tpt;
      tpt.Tot = neededValid;
      tpt.Required = neededValid;
      tpt.Rate = 1.0;
      validRates[it->first] = tpt;
      }

    // Use an otb::OGRDataToSamplePositionFilter with 2 outputs
    PeriodicSamplerType::SamplerParameterType param;
    param.Offset = 0;
    param.MaxJitter = 0;
    PeriodicSamplerType::Pointer splitter = PeriodicSamplerType::New();
    splitter->SetInput(imageList->GetNthElement(i));
    splitter->SetOGRData(source);
    splitter->SetOutputPositionContainerAndRates(destTrain, trainRates, 0);
    splitter->SetOutputPositionContainerAndRates(destValid, validRates, 1);
    splitter->SetFieldName(this->GetParameterString("sample.vfn"));
    splitter->SetLayerIndex(0);
    splitter->SetSamplerParameters(param);
    splitter->GetStreamer()->SetAutomaticTiledStreaming(this->GetParameterInt("ram"));
    AddProcess(splitter->GetStreamer(),"Split samples between training and validation...");
    splitter->Update();
    }

  // train model
  GetInternalApplication("training")->SetParameterStringList("io.vd",sampleTrainOutputs);
  GetInternalApplication("training")->SetParameterStringList("valid.vd",sampleValidOutputs);
  UpdateInternalParameters("training");
  // set field names
  FloatVectorImageType::Pointer image = imageList->GetNthElement(0);
  unsigned int nbBands = image->GetNumberOfComponentsPerPixel();
  std::vector<std::string> selectedNames;
  for (unsigned int i=0 ; i<nbBands ; i++)
    {
    std::ostringstream oss;
    oss << i;
    selectedNames.push_back("value_"+oss.str());
    }
  GetInternalApplication("training")->SetParameterStringList("feat",selectedNames);
  ExecuteInternal("training");
}

};

} // end namespace Wrapper
} // end namespace otb

OTB_APPLICATION_EXPORT(otb::Wrapper::TrainImagesClassifier)
