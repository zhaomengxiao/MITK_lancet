/*=========================================================================
 
Program:   openCherry Platform
Language:  C++
Date:      $Date$
Version:   $Revision$
 
Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.
 
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.
 
=========================================================================*/

#include "cherryQtViewPart.h"

namespace cherry
{

void QtViewPart::CreatePartControl(void* parent)
{
  this->BeforeCreateQtPartControl(static_cast<QWidget*>(parent));
  this->CreateQtPartControl(static_cast<QWidget*>(parent));
  this->AfterCreateQtPartControl(static_cast<QWidget*>(parent));

}

void QtViewPart::BeforeCreateQtPartControl(QWidget* parent)
{

}

void QtViewPart::AfterCreateQtPartControl(QWidget* parent)
{

}

}
