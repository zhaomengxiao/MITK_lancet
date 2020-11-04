# The entries in the mitk_modules list must be
# ordered according to their dependencies.

set(MITK_MODULES
  Core
  CommandLine
  CoreCmdApps
  AppUtil
  LegacyIO
  DataTypesExt
  Annotation
  LegacyGL
  AlgorithmsExt
  MapperExt
  DICOM
  DICOMQI
  DICOMTesting
  SceneSerializationBase
  PlanarFigure
  ImageDenoising
  ImageExtraction
  SceneSerialization
  Gizmo
  GraphAlgorithms
  Multilabel
  Chart
  ImageStatistics
  ContourModel
  SurfaceInterpolation
  Segmentation
  QtWidgets
  QtWidgetsExt
  ImageStatisticsUI
  SegmentationUI
  MatchPointRegistration
  MatchPointRegistrationUI
  Classification
  OpenIGTLink
  IGTBase
  IGT
  CameraCalibration
  OpenCL
  OpenCVVideoSupport
  QtOverlays
  ToFHardware
  ToFProcessing
  ToFUI
  PhotoacousticsHardware
  PhotoacousticsAlgorithms
  PhotoacousticsLib
  US
  USUI
  DICOMUI
  Remeshing
  Python
  QtPython
  PythonService
  QtPythonService
  Persistence
  OpenIGTLinkUI
  IGTUI
  RT
  RTUI
  IOExt
  XNAT
  TubeGraph
  BiophotonicsHardware
  BoundingShape
  RenderWindowManager
  RenderWindowManagerUI
  SemanticRelations
  SemanticRelationsUI
  CEST
  BasicImageProcessing
  ModelFit
  ModelFitUI
  Pharmacokinetics
  PharmacokineticsUI
  DICOMPM
  REST
  RESTService
  DICOMweb
  DeepLearningSegmentation
)

if(MITK_ENABLE_PIC_READER)
  list(APPEND MITK_MODULES IpPicSupportIO)
endif()
