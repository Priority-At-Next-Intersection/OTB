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

#ifndef otbQtLogOutput_h
#define otbQtLogOutput_h

#include <QtWidgets>
#ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829  //tag=QT4-boost-compatibility
#include "itkStdStreamLogOutput.h"
#endif //tag=QT4-boost-compatibility
//#include "itkObjectFactory.h"
//#include "itkProcessObject.h"

#include "OTBQtWidgetExport.h"

namespace otb
{

/** \class QtLogOutput
 *  \brief Class that get log output and launch an event when received.
 *
 *
 * \ingroup OTBQtWidget
 *
 */

class OTBQtWidget_EXPORT QtLogOutput : public QObject, public itk::LogOutput
{
Q_OBJECT
public:
  typedef QtLogOutput                    Self;
  typedef itk::LogOutput                 Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  itkTypeMacro(QtLogOutput, itk::LogOutput);

  itkNewMacro(QtLogOutput);

  /** flush a buffer */
  void Flush() ITK_OVERRIDE;

  /** Write to multiple outputs */
  void Write(double timestamp) ITK_OVERRIDE;

  /** Write to a buffer */
  void Write(std::string const &content) ITK_OVERRIDE;

  /** Write to a buffer */
  void Write(std::string const &content, double timestamp) ITK_OVERRIDE;

signals:
  void NewContentLog(QString);

protected:
  /** Constructor */
  QtLogOutput();

  /** Destructor */
  ~QtLogOutput() ITK_OVERRIDE;

  void PrintSelf(std::ostream &os, itk::Indent indent) const ITK_OVERRIDE;
};

}

#endif
