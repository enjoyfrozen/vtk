set(classes
  vtkIOSSReader
  vtkIOSSCellGridReader
  vtkIOSSWriter
)

set(private_classes
  vtkIOSSCellGridReaderInternal
  vtkIOSSCellGridUtilities
  vtkIOSSFilesScanner
  vtkIOSSUtilities
  vtkIOSSModel
  vtkIOSSReaderInternal
)

set(private_headers
  vtkIOSSReaderCommunication.h
)

vtk_module_add_module(VTK::IOIOSS
  CLASSES ${classes}
  PRIVATE_CLASSES ${private_classes}
  PRIVATE_HEADERS ${private_headers}
)
vtk_add_test_mangling(VTK::IOIOSS)

# vtkIOSSReaderCommunication.h:76:35: error: redefinition... 
set_property(TARGET IOIOSS PROPERTY UNITY_BUILD OFF)
