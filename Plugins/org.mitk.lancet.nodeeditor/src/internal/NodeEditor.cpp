﻿/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

/*=============================================================
Below are Headers for the Node Editor plugin
==============================================================*/
#include <sstream>

#include <cmath>
// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "NodeEditor.h"

// Qt
#include <QDoubleSpinBox>
#include <QPushButton>

// mitk image
#include "basic.h"
#include "mitkImagePixelReadAccessor.h"
#include "mitkImagePixelWriteAccessor.h"
#include "mitkNodePredicateAnd.h"
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include "mitkSurfaceOperation.h"
#include "physioModelFactory.h"
#include <QComboBox>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>
#include <mitkApplyTransformMatrixOperation.h>
#include <mitkBoundingShapeCropper.h>
#include <mitkImage.h>
#include <mitkInteractionConst.h>
#include <mitkPadImageFilter.h>
#include <mitkPointOperation.h>
#include <mitkPointSet.h>
#include <mitkPointSetShapeProperty.h>
#include <mitkRotationOperation.h>
#include <mitkSurface.h>
#include <mitkSurfaceToImageFilter.h>
#include <mitkVector.h>
#include <vtkClipPolyData.h>
#include <vtkImageStencil.h>
#include <vtkLandmarkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkPlane.h>
#include <vtkSTLReader.h>

#include <eigen3/Eigen/Eigen>
#include <mitkPadImageFilter.h>
#include <drr.h>
#include <nodebinder.h>
#include <surfaceregistraion.h>
#include <drrsidonjacobsraytracing.h>

// registration header
#include <twoprojectionregistration.h>

/*=============================================================
Above are Headers for the Node Editor plugin
==============================================================*/

/*=============================================================
Below are Headers for DRR testing
==============================================================*/
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkCenteredEuler3DTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkRescaleIntensityImageFilter.h"
//---

#include "mitkImageCast.h"
#include "itkRayCastInterpolateImageFunction.h"
//---
#include "mitkSurfaceToImageFilter.h"

// void NodeEditor::Polystl2Image()
// {
//   auto stlPolymesh = dynamic_cast<mitk::Surface *>(m_inputCT->GetData());
//   vtkNew<vtkImageData> whiteImage;
//   double bounds[6];
//   stlPolymesh;
// }

void NodeEditor::Polystl2Image()
{
  auto image_to_crop = dynamic_cast<mitk::Image *>(m_inputDRR->GetData());
  auto stlPolymesh = dynamic_cast<mitk::Surface *>(m_inputCT->GetData());
  mitk::Point3D c_image = image_to_crop->GetGeometry()->GetCenter();
  mitk::Point3D c_mesh = stlPolymesh->GetGeometry()->GetOrigin();
  double p[3]{c_mesh[0] - c_image[0], c_mesh[1] - c_image[1], c_mesh[2] - c_image[2]};
  move(p, image_to_crop);
  // stlPolymesh->SetOrigin(c_k);
  
  mitk::Image::Pointer res = mitk::Image::New();
  // stencil
  mitk::SurfaceToImageFilter::Pointer surface_to_image = mitk::SurfaceToImageFilter::New();
  surface_to_image->SetImage(image_to_crop);
  surface_to_image->SetInput(stlPolymesh);
  surface_to_image->SetReverseStencil(false);
  surface_to_image->Update();
  
    // boundingBox
  auto boundingBox = mitk::GeometryData::New();
  // InitializeWithSurfaceGeometry
  auto boundingGeometry = mitk::Geometry3D::New();
  auto geometry = stlPolymesh->GetGeometry();
  boundingGeometry->SetBounds(geometry->GetBounds());
  boundingGeometry->SetOrigin(geometry->GetOrigin());
  boundingGeometry->SetSpacing(geometry->GetSpacing());
  boundingGeometry->SetIndexToWorldTransform(geometry->GetIndexToWorldTransform()->Clone());
  boundingGeometry->Modified();
  boundingBox->SetGeometry(boundingGeometry);
  
  auto cutter = mitk::BoundingShapeCropper::New();
  cutter->SetGeometry(boundingBox);
  cutter->SetInput(surface_to_image->GetOutput());
  cutter->Update();
  res = cutter->GetOutput()->Clone();
  
  QString renameSuffix = "_regPlate";
  QString outputFilename = m_Controls.outputFilename->text();
  auto node = GetDataStorage()->GetNamedNode(outputFilename.toLocal8Bit().data());
  auto newnode = mitk::DataNode::New();
  // in case the output name already exists
  if (node == nullptr)
  {
    newnode->SetName(outputFilename.toLocal8Bit().data());
  }
  else
  {
    newnode->SetName(outputFilename.append(renameSuffix).toLocal8Bit().data());
    m_Controls.outputFilename->setText(outputFilename);
  }
  // add new node
  newnode->SetData(res);
  GetDataStorage()->Add(newnode);

}


void NodeEditor::setDefault()
{
  m_Controls.ledit_gantryRot->setText("0.0");
  m_Controls.camTrans_0->setText("0.0");
  m_Controls.camTrans_1->setText("0.0");
  m_Controls.camTrans_2->setText("0.0");
  m_Controls.rot_0->setText("0");
  m_Controls.rot_1->setText("0.0");
  m_Controls.rot_2->setText("0.0");
  m_Controls.rotCenter_0->setText("0.0");
  m_Controls.rotCenter_1->setText("0.0");
  m_Controls.rotCenter_2->setText("0.0");
  m_Controls.thres->setText("0.0");
  m_Controls.sid->setText("1000");
  m_Controls.outputRes_0->setText("1.0");
  m_Controls.outputRes_1->setText("1.0");
  m_Controls.norm_0->setText("0.0");
  m_Controls.norm_1->setText("0.0");
  m_Controls.outputSize_0->setText("512");
  m_Controls.outputSize_1->setText("512");
  
}

void NodeEditor::drrCustom()
{
  drrGenerateVisual();
  drrGenerateData();
}

void NodeEditor::drrGenerateData()
{
  if (m_inputDRR == nullptr)
  {
    MITK_ERROR << "m_inputDRR null";
    return;
  }
  // the original input image node will be named "unnamed", and you have to rename it because it really does not have a
  // name!!
  // auto image =
  // GetDataStorage()->GetNamedObject<mitk::Image>((m_Controls.inputFilename->text()).toLocal8Bit().data());
  auto image = dynamic_cast<mitk::Image *>(m_inputDRR->GetData());
  // auto sliced = dynamic_cast<mitk::SlicedData *>(m_inputDRR->GetData());

  // auto image = dynamic_cast<mitk::Image *>(sliced);
  // the original input image node will be named "unnamed", and you have to rename it because it really does not have a
  // name!!
  if (image == nullptr)
  {
    MITK_ERROR << "Can't Run DRR: Image null";
    m_Controls.textBrowser_trans_info->append("Error: Input image node is empty");
    return;
  }
  itk::SmartPointer<DRRSidonJacobsRayTracingFilter> drrFilter = DRRSidonJacobsRayTracingFilter::New();

  drrFilter->SetInput(image);

  double rprojection = (m_Controls.ledit_gantryRot->text()).toDouble();
  double tx = (m_Controls.camTrans_0->text()).toDouble();
  double ty = (m_Controls.camTrans_1->text()).toDouble();
  double tz = (m_Controls.camTrans_2->text()).toDouble();
  double rx = (m_Controls.rot_0->text()).toDouble();
  double ry = (m_Controls.rot_1->text()).toDouble();
  double rz = (m_Controls.rot_2->text()).toDouble();
  double cx = (m_Controls.rotCenter_0->text()).toDouble();
  double cy = (m_Controls.rotCenter_1->text()).toDouble();
  double cz = (m_Controls.rotCenter_2->text()).toDouble();
  double threshold = (m_Controls.thres->text()).toDouble();
  double scd = (m_Controls.sid->text()).toDouble();
  double sx = (m_Controls.outputRes_0->text()).toDouble();
  double sy = (m_Controls.outputRes_0->text()).toDouble();
  double o2Dx = (m_Controls.norm_0->text()).toDouble();
  double o2Dy = (m_Controls.norm_1->text()).toDouble();
  int dx = (m_Controls.outputSize_0->text()).toInt();
  int dy = (m_Controls.outputSize_1->text()).toInt();

  drrFilter->Setrprojection(rprojection);
  drrFilter->SetObjTranslate(tx, ty, tz);
  drrFilter->SetObjRotate(rx, ry, rz);
  drrFilter->Setcx(cx);
  drrFilter->Setcy(cy);
  drrFilter->Setcz(cz);
  drrFilter->Setthreshold(threshold);
  drrFilter->Setscd(scd);
  drrFilter->Setim_sx(sx);
  drrFilter->Setim_sy(sy);
  drrFilter->Setdx(dx);
  drrFilter->Setdy(dy);
  drrFilter->Seto2Dx(o2Dx);
  drrFilter->Seto2Dy(o2Dy);
  drrFilter->Update();

  QString renameSuffix = "_new";
  QString outputFilename = m_Controls.outputFilename->text();
  auto node = GetDataStorage()->GetNamedNode(outputFilename.toLocal8Bit().data());
  auto newnode = mitk::DataNode::New();
  // in case the output name already exists
  if (node == nullptr)
  {
    newnode->SetName(outputFilename.toLocal8Bit().data());
  }
  else
  {
    newnode->SetName(outputFilename.append(renameSuffix).toLocal8Bit().data());
    m_Controls.outputFilename->setText(outputFilename);
  }
  // add new node
  newnode->SetData(drrFilter->GetOutput());
  GetDataStorage()->Add(newnode);
}
void NodeEditor::drrGenerateVisual()
{
  if (m_inputDRR == nullptr)
  {
    MITK_ERROR << "m_inputDRR null";
    return;
  }
  // the original input image node will be named "unnamed", and you have to rename it because it really does not have a
  // name!!
  //auto image = GetDataStorage()->GetNamedObject<mitk::Image>((m_Controls.inputFilename->text()).toLocal8Bit().data());
  auto image = dynamic_cast<mitk::Image*>(m_inputDRR->GetData());
  //auto sliced = dynamic_cast<mitk::SlicedData *>(m_inputDRR->GetData());
  
 // auto image = dynamic_cast<mitk::Image *>(sliced);
  //the original input image node will be named "unnamed", and you have to rename it because it really does not have a name!!
  if (image == nullptr)
  {
    MITK_ERROR << "Can't Run DRR: Image null";
    m_Controls.textBrowser_trans_info->append("Error: Input image node is empty");
    return;
  }
  itk::SmartPointer<DRRSidonJacobsRayTracingFilter> drrFilter = DRRSidonJacobsRayTracingFilter::New();


  
  drrFilter->SetInput(image);

  double rprojection = (m_Controls.ledit_gantryRot->text()).toDouble();
  double tx = (m_Controls.camTrans_0->text()).toDouble();
  double ty = (m_Controls.camTrans_1->text()).toDouble();
  double tz = (m_Controls.camTrans_2->text()).toDouble();
  double rx = (m_Controls.rot_0->text()).toDouble();
  double ry = (m_Controls.rot_1->text()).toDouble();
  double rz = (m_Controls.rot_2->text()).toDouble();
  double cx = (m_Controls.rotCenter_0->text()).toDouble();
  double cy = (m_Controls.rotCenter_1->text()).toDouble();
  double cz = (m_Controls.rotCenter_2->text()).toDouble();
  double threshold = (m_Controls.thres->text()).toDouble();
  double scd = (m_Controls.sid->text()).toDouble();
  double sx = (m_Controls.outputRes_0->text()).toDouble();
  double sy = (m_Controls.outputRes_0->text()).toDouble();
  double o2Dx = (m_Controls.norm_0->text()).toDouble();
  double o2Dy = (m_Controls.norm_1->text()).toDouble();
  int dx = (m_Controls.outputSize_0->text()).toInt();
  int dy = (m_Controls.outputSize_1->text()).toInt();

  drrFilter->Setrprojection(rprojection);
  drrFilter->SetObjTranslate(tx, ty, tz);
  drrFilter->SetObjRotate(rx, ry, rz);
  drrFilter->Setcx(cx);
  drrFilter->Setcy(cy);
  drrFilter->Setcz(cz);
  drrFilter->Setthreshold(threshold);
  drrFilter->Setscd(scd);
  drrFilter->Setim_sx(sx);
  drrFilter->Setim_sy(sy);
  drrFilter->Setdx(dx);
  drrFilter->Setdy(dy);
  drrFilter->Seto2Dx(o2Dx);
  drrFilter->Seto2Dy(o2Dy);
  drrFilter->Update();

  
  QString renameSuffix = "_new";
  QString outputFilename = m_Controls.outputFilename->text();
  auto node = GetDataStorage()->GetNamedNode(outputFilename.toLocal8Bit().data());
  auto newnode = mitk::DataNode::New();
  // in case the output name already exists
  if (node == nullptr)
  {    
    newnode->SetName(outputFilename.toLocal8Bit().data());
  }
  else
  {    
    newnode->SetName(outputFilename.append(renameSuffix).toLocal8Bit().data());
    m_Controls.outputFilename->setText(outputFilename);
  }


  //add new node for DRR geometry visualization


  mitk::Image::Pointer image_trans = drrFilter->GetOutput();
  mitk::Point3D c_k = m_inputDRR->GetData()->GetGeometry()->GetCenter();

  double c_v[3]{c_k[0], c_k[1], c_k[2]};

  // rotate 90 degrees to fit the DRR geometry
  double x_axis[3]{1, 0, 0};
  double isoc[3]{0, 0, -scd};
  rotate(isoc, x_axis, -90, image_trans);

  // move the center of the image to the isocenter in the sample coordinates
  double p[3]{c_v[0]+cx, c_v[1]+cy  ,c_v[2]+cy + scd }; // translation vector
  // mitk::Point3D direciton{p};
  move(p, image_trans);

  
  double isocw[3]{c_v[0] + cx, c_v[1] + cy, c_v[2] + cz };

  

  m_Controls.ledit_isox->setText(QString::number(isocw[0]));
  m_Controls.ledit_isoy->setText(QString::number(isocw[1]));
  m_Controls.ledit_isoz->setText(QString::number(isocw[2]));

  // move the image by some -y for better visualization
  double p_1[3]{0, scd, 0};
  move(p_1, image_trans);

  // gantry rotation offset
  double z_axis[3]{0, 0, 1};
  rotate(isocw, z_axis, rprojection, image_trans);

  // mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  
  auto geo_node = mitk::DataNode::New();
  QString geo_Suffix = "_visual";
  geo_node->SetName(outputFilename.append(geo_Suffix).toLocal8Bit().data());
  geo_node->SetData(image_trans);
  GetDataStorage()->Add(geo_node);

  if (m_Controls.rdb_moveCT->isChecked()){
  // add a node that contains the moved CT image
  itk::Image<short, 3>::Pointer m_movedCTimage;
  mitk::Image::Pointer image_tmp;
  mitk::CastToItkImage(image, m_movedCTimage);
  mitk::CastToMitkImage(m_movedCTimage, image_tmp);
  double Z_axis[3]{0, 0, 1};
  rotate(isoc, Z_axis, rz, image_tmp);
  double Y_axis[3]{0, 1, 0};
  rotate(isoc, Y_axis, ry, image_tmp);
  double X_axis[3]{1, 0, 0};
  rotate(isoc, X_axis, rz, image_tmp);
  double p_tmp[3]{tx, ty, tz};
  move(p_tmp, image_tmp);

  auto movedCT_node = mitk::DataNode::New();
  QString movedCT_Suffix = "_sample";
  movedCT_node->SetName(outputFilename.append(movedCT_Suffix).toLocal8Bit().data());
  movedCT_node->SetData(image_tmp);
  GetDataStorage()->Add(movedCT_node);
  }

  c_v[0] =(c_v[0] + cx)+ scd * sin(rprojection*3.1415926/180);
  c_v[1] = (c_v[1] + cy) - scd * cos(rprojection * 3.1415926 / 180);
  c_v[2] = c_v[2] + cz;
  // double xsourcew[3]{c_v[0] + cx, c_v[1] + cy - scd, c_v[2] + cz};
  

  m_Controls.ledit_srcx->setText(QString::number(c_v[0]));
  m_Controls.ledit_srcy->setText(QString::number(c_v[1]));
  m_Controls.ledit_srcz->setText(QString::number(c_v[2]));
}




void NodeEditor::move(double d[3], mitk::Image *image_visual)
{
  if (image_visual != nullptr)
  {
    mitk::Point3D direciton{d};
    auto *doOp = new mitk::PointOperation(mitk::OpMOVE, 0, direciton, 0);
    // execute the Operation
    // here no undo is stored, because the movement-steps aren't interesting.
    // only the start and the end is interisting to store for undo.
    image_visual->GetGeometry()->ExecuteOperation(doOp);
    
    
    delete doOp;
    // updateStemCenter();

    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  }
}

void NodeEditor::rotate(double center[3], double axis[3], double degree, mitk::Image *image_visual)
{
  if (image_visual != nullptr)
  {
    mitk::Point3D rotateCenter{center};
    mitk::Vector3D rotateAxis{axis};
    auto *doOp = new mitk::RotationOperation(mitk::OpROTATE, rotateCenter, rotateAxis, degree);
    // execute the Operation
    // here no undo is stored, because the movement-steps aren't interesting.
    // only the start and the end is interisting to store for undo.
    image_visual->GetGeometry()->ExecuteOperation(doOp);
    delete doOp;
    // updateStemCenter();
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
  }
}




/*=============================================================
Above is the Code for DRR
==============================================================*/


//-------------------------------- ↓  registration part  ↓---------------------------------------
void NodeEditor::twoProjectRegister() 
{
  if (m_inputCT == nullptr || m_inputDRR1 == nullptr || m_inputDRR2 == nullptr)
  {
    MITK_ERROR << "Input nodes are not ready";
    return;
  }

  auto ctimage = dynamic_cast<mitk::Image *>(m_inputCT->GetData());
  auto DRR1 = dynamic_cast<mitk::Image *>(m_inputDRR1->GetData());
  auto DRR2 = dynamic_cast<mitk::Image *>(m_inputDRR2->GetData());


  if (ctimage == nullptr || DRR1 == nullptr || DRR2 == nullptr)
  {
    MITK_ERROR << "Can't Run twoProjectionRegistration: Input images are empty";
    m_Controls.textBrowser_trans_info->append("Error: Input image node is empty");
    return;
  }
  itk::SmartPointer<TwoProjectionRegistration> registrator = TwoProjectionRegistration::New();

  registrator->link_drr1_cast(DRR1);
  registrator->link_drr2_cast(DRR2);
  registrator->link_3d_cast(ctimage);

  

  double angleDRR1 = (m_Controls.ledit_angleDRR1->text()).toDouble();
  double angleDRR2 = (m_Controls.ledit_angleDRR2->text()).toDouble();
  double tx = (m_Controls.ledit_tx_reg->text()).toDouble();
  double ty = (m_Controls.ledit_tx_reg->text()).toDouble();
  double tz = (m_Controls.ledit_tx_reg->text()).toDouble();
  double cx = (m_Controls.ledit_cx_reg->text()).toDouble();
  double cy = (m_Controls.ledit_cy_reg->text()).toDouble();
  double cz = (m_Controls.ledit_cz_reg->text()).toDouble();
  double rx = (m_Controls.ledit_rx_reg->text()).toDouble();
  double ry = (m_Controls.ledit_ry_reg->text()).toDouble();
  double rz = (m_Controls.ledit_rz_reg->text()).toDouble();
  double threshold = (m_Controls.ledit_thres_reg->text()).toDouble();
  double scd = (m_Controls.ledit_scd->text()).toDouble();
  double sx_1 = (m_Controls.ledit_DRR1res_x->text()).toDouble();
  double sy_1= (m_Controls.ledit_DRR1res_y->text()).toDouble();
  double sx_2 = (m_Controls.ledit_DRR2res_x->text()).toDouble();
  double sy_2 = (m_Controls.ledit_DRR2res_y->text()).toDouble();
  double o2Dx_1 = (m_Controls.ledit_2dcxDRR1_x_reg->text()).toDouble();
  double o2Dy_1 = (m_Controls.ledit_2dcxDRR1_y_reg->text()).toDouble();
  double o2Dx_2 = (m_Controls.ledit_2dcxDRR2_x_reg->text()).toDouble();
  double o2Dy_2 = (m_Controls.ledit_2dcxDRR2_y_reg->text()).toDouble();

  if (sx_1 == 0 || sy_1 || sx_2 == 0 || sy_2==0)
  {
    std::cout << "FLAG!" << std::endl;
  }

  registrator->SetangleDRR1(angleDRR1);
  registrator->SetangleDRR2(angleDRR2);
  registrator->Settx(tx);
  registrator->Setty(ty);
  registrator->Settz(tz);
  registrator->Setcx(cx);
  registrator->Setcy(cy);
  registrator->Setcz(cz);
  registrator->Setrx(rx);
  registrator->Setry(ry);
  registrator->Setrz(rz);
  registrator->Setthreshold(threshold);
  registrator->Setscd(scd);
  registrator->Setsx_1(sx_1);
  registrator->Setsy_1(sy_1);
  registrator->Setsx_2(sx_2);
  registrator->Setsy_2(sy_2);
  registrator->Seto2Dx_1(o2Dx_1);
  registrator->Seto2Dy_1(o2Dy_1);
  registrator->Seto2Dx_2(o2Dx_2);
  registrator->Seto2Dy_2(o2Dy_2);

  registrator->twoprojection_registration();

  m_Controls.ledit_RX->setText(QString::number(registrator->GetRX()));
  m_Controls.ledit_RY->setText(QString::number(registrator->GetRY()));
  m_Controls.ledit_RZ->setText(QString::number(registrator->GetRZ()));

  m_Controls.ledit_TX->setText(QString::number(registrator->GetTX()));
  m_Controls.ledit_TY->setText(QString::number(registrator->GetTY()));
  m_Controls.ledit_TZ->setText(QString::number(registrator->GetTZ()));
}

//-------------------------------- ↑  registration part  ↑---------------------------------------



//-------------------------------- ↓  QT part  ↓---------------------------------------

#define PI acos(-1)
const std::string NodeEditor::VIEW_ID = "org.mitk.views.nodeeditor";

void NodeEditor::SetFocus()
{
  // m_Controls.pushButton_applyLandMark->setFocus();
}

void NodeEditor::InitPointSetSelector(QmitkSingleNodeSelectionWidget *widget)
{
  widget->SetDataStorage(GetDataStorage());
  widget->SetNodePredicate(mitk::NodePredicateAnd::New(
    mitk::TNodePredicateDataType<mitk::PointSet>::New(),
    mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
                                                           mitk::NodePredicateProperty::New("hidden object")))));

  widget->SetSelectionIsOptional(true);
  widget->SetAutoSelectNewNodes(true);
  widget->SetEmptyInfo(QString("Please select a point set"));
  widget->SetPopUpTitel(QString("Select point set"));
}

void NodeEditor::InitNodeSelector(QmitkSingleNodeSelectionWidget *widget)
{
  widget->SetDataStorage(GetDataStorage());
  widget->SetNodePredicate(mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(
    mitk::NodePredicateProperty::New("helper object"), mitk::NodePredicateProperty::New("hidden object"))));
  widget->SetSelectionIsOptional(true);
  widget->SetAutoSelectNewNodes(true);
  widget->SetEmptyInfo(QString("Please select a node"));
  widget->SetPopUpTitel(QString("Select node"));
}


void NodeEditor::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);
  //connect(m_Controls.buttonPerformImageProcessing, &QPushButton::clicked, this, &NodeEditor::DoImageProcessing);

  // Set Node Selection Widget
  
  InitNodeSelector(m_Controls.widget_DRR);
  InitNodeSelector(m_Controls.widget_inputCT_regis);
  InitNodeSelector(m_Controls.widget_inputDRR1_regis);
  InitNodeSelector(m_Controls.widget_inputDRR2_regis);
  // InitNodeSelector(m_Controls.widget_stl);



  //drr
  connect(m_Controls.btn_drrTest, &QPushButton::clicked, this, &NodeEditor::setDefault);
  connect(m_Controls.btn_drrCustom,&QPushButton::clicked, this, &NodeEditor::drrCustom);
  connect(m_Controls.widget_DRR,
          &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged,
          this,
          &NodeEditor::inputDRRChanged);

  //twoProjectionRegistration
  connect(m_Controls.btn_reg, &QPushButton::clicked, this, &NodeEditor::twoProjectRegister);
  connect(m_Controls.widget_inputCT_regis,
          &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged,
          this,
          &NodeEditor::inputCTChanged);
  connect(m_Controls.widget_inputDRR1_regis,
          &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged,
          this,
          &NodeEditor::inputDRR1Changed);
  connect(m_Controls.widget_inputDRR2_regis,
          &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged,
          this,
          &NodeEditor::inputDRR2Changed);
  //stl polydata to imagedata
  connect(m_Controls.btn_stlImage, &QPushButton::clicked, this, &NodeEditor::Polystl2Image);
  // connect(m_Controls.widget_stl,
  //         &QmitkSingleNodeSelectionWidget::CurrentSelectionChanged,
  //         this,
  //         &NodeEditor::inputstldataChanged);
  

}

void NodeEditor::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
                                                const QList<mitk::DataNode::Pointer> &nodes)
{

}

//-------------------------------- ↑  QT part  ↑---------------------------------------


