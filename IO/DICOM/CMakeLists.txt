# GDCM or DCMTK are required for reading compressed DICOM images
# (only turn on one of these, not both!)
option(VTK_USE_GDCM "Use GDCM for DICOM image decompression" OFF)
option(VTK_USE_DCMTK "Use DCMTK for DICOM image decompression" OFF)

# Configuration header
set(DICOM_USE_GDCM ${VTK_USE_GDCM})
set(DICOM_USE_DCMTK ${VTK_USE_DCMTK})
set(DICOM_USE_SQLITE OFF)
if(TARGET VTK::sqlite)
  set(DICOM_USE_SQLITE ON)
endif()
configure_file(vtkDICOMConfig.h.in vtkDICOMConfig.h)

set(classes
  vtkDICOMMetaData
  vtkDICOMDictionary
  vtkDICOMFilePath
  vtkDICOMFile
  vtkDICOMFileDirectory
  vtkDICOMTag
  vtkDICOMTagPath
  vtkDICOMVR
  vtkDICOMVM
  vtkDICOMCharacterSet
  vtkDICOMCharacterSetTables
  vtkDICOMDataElement
  vtkDICOMDictHash
  vtkDICOMDictEntry
  vtkDICOMDictPrivate
  vtkDICOMDirectory
  vtkDICOMFileSorter
  vtkDICOMGenerator
  vtkDICOMImageCodec
  vtkDICOMSCGenerator
  vtkDICOMCTGenerator
  vtkDICOMMRGenerator
  vtkDICOMParser
  vtkDICOMCompiler
  vtkDICOMReader
  vtkDICOMSliceSorter
  vtkDICOMSequence
  vtkDICOMItem
  vtkDICOMSorter
  vtkDICOMUtilities
  vtkDICOMUtilitiesUIDTable
  vtkDICOMValue
  vtkDICOMWriter
  vtkDICOMAlgorithm
  vtkDICOMLookupTable
  vtkDICOMApplyPalette
  vtkDICOMApplyRescale
  vtkDICOMToRAS
  vtkDICOMCTRectifier
  vtkDICOMMetaDataAdapter
  vtkDICOMUIDGenerator
)

set(headers
  ${CMAKE_CURRENT_BINARY_DIR}/vtkDICOMConfig.h
  ${CMAKE_CURRENT_BINARY_DIR}/vtkIODICOMModule.h
)

set(GDCM_LIBS)
if(VTK_USE_GDCM)
  set(_components gdcmMSFF gdcmIOD gdcmDSED gdcmDICT)
  vtk_module_find_package(
    PACKAGE GDCM
    COMPONENTS ${_components})
  include(${GDCM_USE_FILE})
  set(GDCM_LIBS ${_components})
endif()

set(DCMTK_LIBS)
if(VTK_USE_DCMTK)
  vtk_module_find_package(PACKAGE DCMTK)
  if(${DCMTK_charls_LIBRARY})
    set(DCMTK_LIBS ${DCMTK_LIBRARIES} ${DCMTK_charls_LIBRARY})
  else()
    set(DCMTK_LIBS ${DCMTK_LIBRARIES})
  endif()
  if(APPLE)
    list(APPEND DCMTK_LIBS iconv)
  endif()
  include_directories(${DCMTK_INCLUDE_DIRS})
endif()

vtk_module_add_module(VTK::IODICOM
  CLASSES ${classes}
  HEADERS ${headers})
vtk_module_link(VTK::IODICOM
  PRIVATE ${GDCM_LIBS} ${DCMTK_LIBS})

if(VTK_BUILD_EXAMPLES)
  add_subdirectory(Examples/Cxx)
endif()
