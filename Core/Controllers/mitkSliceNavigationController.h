#ifndef SLICENAVIGATIONCONTROLLER_H_HEADER_INCLUDED_C1C55A2F
#define SLICENAVIGATIONCONTROLLER_H_HEADER_INCLUDED_C1C55A2F

#include "mitkCommon.h"
#include "mitkBaseController.h"
#include "mitkImageToImageFilter.h"

namespace mitk {

//##ModelId=3DD523E00048
//##Documentation
//## @brief controls the selection of the slice the associated BaseRenderer
//## will display
//## @ingroup NavigationControl
//## Subclass of BaseController. Controls the selection of the slice the
//## associated BaseRenderer will display.
class SliceNavigationController : public BaseController
{
public:
  mitkClassMacro(SliceNavigationController,BaseController);
  itkNewMacro(Self);

  enum ViewDirection{Transversal, Sagittal, Frontal, Original};

  //##ModelId=3DD524D7038C
  const mitk::Geometry3D* GetGeometry();

  //##ModelId=3E3AA1E20393
  virtual void SetGeometry(const mitk::Geometry3D* aGeometry3D);

  //##ModelId=3DD524F9001A
  virtual void SetGeometry2D(const mitk::Geometry2D* aGeometry2D);

  itkSetMacro(ViewDirection, ViewDirection);

  itkGetMacro(ViewDirection, ViewDirection);

  virtual bool AddRenderer(mitk::BaseRenderer* renderer);

protected:
  //##ModelId=3E189B1D008D
  SliceNavigationController();

  //##ModelId=3E189B1D00BF
  virtual ~SliceNavigationController();

  virtual void SliceStepperChanged();

  virtual void TimeStepperChanged();

  mitk::Geometry3D::ConstPointer m_Geometry3D;

  mitk::Geometry2D::ConstPointer m_Geometry2D;

  mitk::PlaneGeometry::Pointer m_Plane;

  Vector3D m_NormalizedSliceDirection;

  ViewDirection m_ViewDirection;

  bool m_BlockUpdate;
};

} // namespace mitk



#endif /* SLICENAVIGATIONCONTROLLER_H_HEADER_INCLUDED_C1C55A2F */
