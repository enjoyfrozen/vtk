set(test_exclusions
  # https://gitlab.kitware.com/vtk/vtk/-/issues/19427
  "^VTK::RenderingOpenGL2Cxx-TestGlyph3DMapperPickability$")

if (NOT "$ENV{CMAKE_CONFIGURATION}" MATCHES "windows")
  list(APPEND test_exclusions
    # Flaky; timesout sometimes on macOS and Linux
    "^VTK::RenderingVolumeOpenGL2Cxx-TestGPURayCastDepthPeelingBoxWidget$"
  )
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "fedora" OR
    "$ENV{CMAKE_CONFIGURATION}" MATCHES "el8")
  list(APPEND test_exclusions
    # See this issue to track the status of these tests.
    # https://gitlab.kitware.com/vtk/vtk/-/issues/18098

    # Line rendering differences
    "^VTK::FiltersCorePython-contourCells$"
    "^VTK::FiltersCorePython-contourQuadraticCells$"
    "^VTK::FiltersFlowPathsCxx-TestBSPTree$"
    "^VTK::FiltersGeneralCxx-TestDensifyPolyData$" # valid image looks weird too
    "^VTK::FiltersGeneralPython-clipQuadraticCells$"
    "^VTK::FiltersGeneralPython-edgePoints$"
    "^VTK::FiltersGeneralPython-TestFEDiscreteClipper2D$"
    "^VTK::FiltersGeometryCxx-TestLinearToQuadraticCellsFilter$"
    "^VTK::FiltersHyperTreeCxx-TestHyperTreeGridTernary3DDualContourMaterial$"
    "^VTK::FiltersHyperTreeCxx-TestHyperTreeGridTernary3DGeometryLargeMaterialBits$"
    "^VTK::FiltersHyperTreeCxx-TestHyperTreeGridTernary3DPlaneCutterDual$"
    "^VTK::FiltersModelingPython-TestCookieCutter3$"
    "^VTK::FiltersModelingPython-TestImprintFilter3$"
    "^VTK::FiltersModelingPython-TestImprintFilter6$"
    "^VTK::FiltersSourcesPython-TestStaticCellLocatorLineIntersection$"
    "^VTK::InteractionWidgetsCxx-TestPickingManagerWidgets$"
    "^VTK::InteractionWidgetsPython-TestTensorWidget2$"
    "^VTK::RenderingOpenGL2Cxx-TestCoincident$"
    "^VTK::RenderingOpenGL2Python-TestTopologyResolution$"
    "^VTK::RenderingVolumeCxx-TestGPURayCastMapperRectilinearGrid$"

    # Timeout; needs investigation
    "^VTK::RenderingOpenGL2Cxx-TestFloor$"

    # Point rendering differences
    "^VTK::FiltersPointsPython-TestConnectedPointsFilter$" # other differences too
    "^VTK::FiltersPointsPython-TestPCANormalEstimation$"
    "^VTK::FiltersPointsPython-TestPCANormalEstimation2$"
    "^VTK::FiltersPointsPython-TestRadiusOutlierRemoval$"
    "^VTK::FiltersPointsPython-TestVoxelGridFilter$"
    "^VTK::IOGeometryPython-ParticleReader$"
    "^VTK::InteractionWidgetsPython-TestPointCloudWidget$"
    "^VTK::InteractionWidgetsPython-TestPointCloudWidget2$"

    # Floating point imprecision?
    "^VTK::FiltersGeneralPython-TestSampleImplicitFunctionFilter$"

    # Test image looks "dim"; image rendering seems to be common
    # (some also have vertical line rendering differences)
    "^VTK::FiltersModelingPython-TestCookieCutter$"
    "^VTK::RenderingCoreCxx-TestTextureRGBADepthPeeling$" # seems to just not work here

    # Font rendering differences (new baseline?)
    "^VTK::RenderingFreeTypeCxx-TestFreeTypeTextMapperWithColumns$"

    # Flaky timeouts https://gitlab.kitware.com/vtk/vtk/-/issues/18861
    "^VTK::InteractionWidgetsCxx-TestPickingManagerSeedWidget$"

    # Flaky failures https://gitlab.kitware.com/vtk/vtk/-/issues/19040
    "^VTK::ViewsInfovisCxx-TestGraphLayoutView$"
    "^VTK::ViewsInfovisCxx-TestRenderView$"
    )
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "el8")
  list(APPEND test_exclusions
    # Matplotlib fails to render anything. See #19302.
    "^VTK::RenderingMatplotlibCxx-TestRenderString$"
    "^VTK::RenderingMatplotlibCxx-TestScalarBarCombinatorics$"
    "^VTK::RenderingMatplotlibCxx-TestStringToPath$"
    "^VTK::RenderingMatplotlibPython-TestMathTextActor$"
    "^VTK::RenderingMatplotlibPython-TestMathTextActor3D$"
    "^VTK::RenderingMatplotlibPython-TestRenderString$"
    "^VTK::RenderingMatplotlibPython-TestStringToPath$"

    # Consistent timeout. Needs investigation.
    # https://gitlab.kitware.com/vtk/vtk/-/issues/19303
    "^VTK::RenderingOpenGL2Cxx-TestFloor$"

    # Intermittent flakiness; may be related to CI runner OpenGL config.
    # Appears as a colormap or color-range failure:
    "^VTK::FiltersCellGridPython-TestUnstructuredGridToCellGrid$"
    )
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "fedora")
  list(APPEND test_exclusions
    # See this issue to track the status of these tests.
    # https://gitlab.kitware.com/vtk/vtk/-/issues/18098

    # Point rendering differences
    "^VTK::IOLASCxx-TestLASReader_test_1$"
    "^VTK::IOLASCxx-TestLASReader_test_2$"
    "^VTK::IOPDALCxx-TestPDALReader_test_1$"
    "^VTK::IOPDALCxx-TestPDALReader_test_2$"

    # Numerical problems?
    "^VTK::FiltersOpenTURNSCxx-TestOTKernelSmoothing$"

    # Syntax error in generated shader program.
    "^VTK::RenderingExternalCxx-TestGLUTRenderWindow$"

    # Flaky timeouts
    # https://gitlab.kitware.com/vtk/vtk/-/issues/18984
    "^VTK::ViewsInfovisCxx-TestGraphLayoutView$"

    # Rendering in the wrong order.
    "^VTK::InteractionWidgetsCxx-TestResliceCursorWidget2$"
    "^VTK::InteractionWidgetsCxx-TestResliceCursorWidget3$"

    # MPI detects bad memory handling
    "^VTK::IOPIOPython-MPI-TestPIOReader$"
    )
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "fedora39" AND "$ENV{CMAKE_CONFIGURATION}" MATCHES "mpi")
  list(APPEND test_exclusions
    # MPI initialization failures from inside of IOSS. Needs investigation.
    # https://gitlab.kitware.com/vtk/vtk/-/issues/19314
    "^VTK::DomainsParallelChemistryCxx-MPI-TestPSimpleBondPerceiver$"
    "^VTK::FiltersCellGridPython-TestUnstructuredGridToCellGrid$"
    "^VTK::FiltersCoreCxx-TestAppendSelection$"
    "^VTK::FiltersCoreCxx-TestDecimatePolylineFilter$"
    "^VTK::FiltersCoreCxx-TestFeatureEdges$"
    "^VTK::FiltersCorePython-TestCompositeDataSetPlaneCutter$"
    "^VTK::FiltersExtractionCxx-TestExpandMarkedElements$"
    "^VTK::FiltersExtractionCxx-TestExtractionExpression$"
    "^VTK::FiltersExtractionCxx-TestExtractSelectionUsingDataAssembly$"
    "^VTK::FiltersGeneralCxx-TestAnimateModes$"
    "^VTK::FiltersGeneralCxx-TestGradientAndVorticity$"
    "^VTK::FiltersHybridCxx-TestTemporalCacheUndefinedTimeStep$"
    "^VTK::FiltersHybridCxx-TestTemporalInterpolator$"
    "^VTK::FiltersHybridCxx-TestTemporalInterpolatorFactorMode$"
    "^VTK::FiltersParallelCxx-MPI-AggregateDataSet$"
    "^VTK::FiltersParallelCxx-MPI-DistributedData$"
    "^VTK::FiltersParallelCxx-MPI-DistributedDataRenderPass$"
    "^VTK::FiltersParallelCxx-MPI-ParallelResampling$"
    "^VTK::FiltersParallelCxx-MPI-PTextureMapToSphere$"
    "^VTK::FiltersParallelCxx-MPI-TestGenerateGlobalIdsHTG$"
    "^VTK::FiltersParallelCxx-MPI-TestGenerateProcessIds$"
    "^VTK::FiltersParallelCxx-MPI-TestGenerateProcessIdsHTG$"
    "^VTK::FiltersParallelCxx-MPI-TestHyperTreeGridGhostCellsGenerator$"
    "^VTK::FiltersParallelCxx-MPI-TestPartitionBalancer$"
    "^VTK::FiltersParallelCxx-MPI-TestPExtractDataArraysOverTime$"
    "^VTK::FiltersParallelCxx-MPI-TestPHyperTreeGridProbeFilter$"
    "^VTK::FiltersParallelCxx-MPI-TestPOutlineFilter$"
    "^VTK::FiltersParallelCxx-MPI-TransmitImageData$"
    "^VTK::FiltersParallelCxx-MPI-TransmitImageDataRenderPass$"
    "^VTK::FiltersParallelCxx-MPI-TransmitRectilinearGrid$"
    "^VTK::FiltersParallelCxx-MPI-TransmitStructuredGrid$"
    "^VTK::FiltersParallelCxx-MPI-UnitTestPMaskPoints$"
    "^VTK::FiltersParallelCxx-TestPOutlineFilter$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-DIYAggregateDataSet$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestAdaptiveResampleToImage$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestDIYGenerateCuts$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestGenerateGlobalIds$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestGhostCellsGenerator$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestOverlappingCellsDetector$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestPResampleHyperTreeGridWithDataSet$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestPResampleToImage$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestPResampleToImageCompositeDataSet$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestPResampleWithDataSet$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestPResampleWithDataSet2$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestProbeLineFilter$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestPUniformGridGhostDataGenerator$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestPUnstructuredGridGhostCellsGenerator$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestRedistributeDataSetFilter$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestRedistributeDataSetFilterImplicitArray$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestRedistributeDataSetFilterOnIoss$"
    "^VTK::FiltersParallelDIY2Cxx-TestAdaptiveResampleToImage$"
    "^VTK::FiltersParallelDIY2Cxx-TestExtractSubsetWithSeed$"
    "^VTK::FiltersParallelDIY2Cxx-TestGenerateGlobalIds$"
    "^VTK::FiltersParallelDIY2Cxx-TestGenerateGlobalIdsSphere$"
    "^VTK::FiltersParallelDIY2Cxx-TestOverlappingCellsDetector$"
    "^VTK::FiltersParallelDIY2Cxx-TestRedistributeDataSetFilter$"
    "^VTK::FiltersParallelDIY2Cxx-TestRedistributeDataSetFilterOnIoss$"
    "^VTK::FiltersParallelDIY2Cxx-TestRedistributeDataSetFilterOnIoss$"
    "^VTK::FiltersParallelDIY2Cxx-TestRedistributeDataSetFilterWithPolyData$"
    "^VTK::FiltersParallelDIY2Cxx-TestStitchImageDataWithGhosts$"
    "^VTK::FiltersParallelDIY2Cxx-TestUniformGridGhostDataGenerator$"
    "^VTK::FiltersParallelFlowPathsCxx-MPI-TestPLagrangianParticleTracker$"
    "^VTK::FiltersParallelFlowPathsCxx-MPI-TestPParticleTracers$"
    "^VTK::FiltersParallelFlowPathsCxx-MPI-TestPStream$"
    "^VTK::FiltersParallelFlowPathsCxx-MPI-TestPStreamAMR$"
    "^VTK::FiltersParallelFlowPathsCxx-MPI-TestPStreamGeometry$"
    "^VTK::FiltersParallelGeometryCxx-MPI-ParallelConnectivity1$"
    "^VTK::FiltersParallelGeometryCxx-MPI-ParallelConnectivity4$"
    "^VTK::FiltersParallelGeometryCxx-MPI-TestPolyhedralMeshDistributedDataFilter$"
    "^VTK::FiltersParallelGeometryCxx-MPI-TestPStructuredGridConnectivity$"
    "^VTK::FiltersParallelMPICxx-MPI-TestDistributedPointCloudFilter1$"
    "^VTK::FiltersParallelMPICxx-MPI-TestDistributedPointCloudFilter2$"
    "^VTK::FiltersParallelMPICxx-MPI-TestDistributedPointCloudFilter5$"
    "^VTK::FiltersParallelMPICxx-MPI-TestImplicitConnectivity$"
    "^VTK::FiltersParallelStatisticsCxx-MPI-TestPCorrelativeStatistics$"
    "^VTK::FiltersParallelStatisticsCxx-MPI-TestPDescriptiveStatistics$"
    "^VTK::FiltersParallelStatisticsCxx-MPI-TestRandomPContigencyStatisticsMPI$"
    "^VTK::FiltersParallelStatisticsCxx-MPI-TestRandomPContingencyStatisticsMPI$"
    "^VTK::FiltersParallelStatisticsCxx-MPI-TestRandomPKMeansStatisticsMPI$"
    "^VTK::FiltersParallelStatisticsCxx-MPI-TestRandomPMomentStatisticsMPI$"
    "^VTK::FiltersParallelStatisticsCxx-MPI-TestRandomPOrderStatisticsMPI$"
    "^VTK::FiltersParallelVerdictCxx-MPI-PCellSizeFilter$"
    "^VTK::FiltersSourcesCxx-MPI-TestRandomHyperTreeGridSourceMPI3$"
    "^VTK::FiltersSourcesCxx-MPI-TestSpatioTemporalHarmonicsSourceDistributed$"
    "^VTK::IOADIOS2Cxx-MPI-TestADIOS2BPReaderMPIMultiTimeSteps2D$"
    "^VTK::IOADIOS2Cxx-TestIOADIOS2VTX_VTI3DRendering$"
    "^VTK::IOADIOS2Cxx-TestIOADIOS2VTX_VTU1DRendering$"
    "^VTK::IOADIOS2Cxx-TestIOADIOS2VTX_VTU2DRendering$"
    "^VTK::IOADIOS2Cxx-TestIOADIOS2VTX_VTU3DRendering$"
    "^VTK::IOADIOS2Cxx-UnitTestIOADIOS2VTX$"
    "^VTK::IOCatalystConduitCxx-MPI-TestConduitSource$"
    "^VTK::IOCatalystConduitCxx-TestConduitSource$"
    "^VTK::IOFidesPython-TestFidesBasic$"
    "^VTK::IOHDFCxx-MPI-TestHDFWriterDistributed$"
    "^VTK::IOIOSSCxx-MPI-TestIOSSCatalystExodus$"
    "^VTK::IOIOSSCxx-MPI-TestIOSSExodusParallelWriter$"
    "^VTK::IOIossCxx-MPI-TestIossExodusParitionedFiles$"
    "^VTK::IOIOSSCxx-MPI-TestIOSSExodusPartitionedFiles$"
    "^VTK::IOIossCxx-MPI-TestIossExodusRestarts$"
    "^VTK::IOIossCxx-TestIossApplyDisplacementsCGNS$"
    "^VTK::IOIossCxx-TestIossAssemblies$"
    "^VTK::IOIossCxx-TestIossAttributes$"
    "^VTK::IOIOSSCxx-TestIOSSCatalystCGNS$"
    "^VTK::IOIOSSCxx-TestIOSSCatalystExodus$"
    "^VTK::IOIossCxx-TestIossCGNS$"
    "^VTK::IOIossCxx-TestIossExodus$"
    "^VTK::IOIOSSCxx-TestIOSSExodusMergeEntityBlocks$"
    "^VTK::IOIOSSCxx-TestIOSSExodusParallelWriter$"
    "^VTK::IOIossCxx-TestIossExodusRestarts$"
    "^VTK::IOIOSSCxx-TestIOSSExodusSetArrays$"
    "^VTK::IOIOSSCxx-TestIOSSExodusWriter$"
    "^VTK::IOIOSSCxx-TestIOSSExodusWriterClip$"
    "^VTK::IOIOSSCxx-TestIOSSExodusWriterCrinkleClip$"
    "^VTK::IOIOSSCxx-TestIOSSGhostArray$"
    "^VTK::IOIossCxx-TestIossNoElementBlocks$"
    "^VTK::IOIOSSCxx-TestIOSSReadAllFilesToDetermineStructure$"
    "^VTK::IOIossCxx-TestIossSuperelements$"
    "^VTK::IOIossCxx-TestIossTri6$"
    "^VTK::IOIossCxx-TestIossUnsupported$"
    "^VTK::IOIOSSCxx-TestIOSSWedge21$"
    "^VTK::IOIOSSPython-TestIOSSCellGridReader$"
    "^VTK::IOMPIParallelPython-MPI-Plot3DMPIIO$"
    "^VTK::IOParallelCxx-MPI-TestPOpenFOAMReaderLagrangianUncollated$"
    "^VTK::ParallelDIYCxx-MPI-TestDIYDataExchanger$"
    "^VTK::ParallelDIYCxx-MPI-TestDIYUtilities$"
    "^VTK::ParallelMPICxx-MPI-MPIController$"
    "^VTK::ParallelMPICxx-MPI-PDirectory$"
    "^VTK::ParallelMPICxx-MPI-PSystemTools$"
    "^VTK::ParallelMPICxx-MPI-TestNonBlockingCommunication$"
    "^VTK::ParallelMPICxx-MPI-TestPProbe$"
    "^VTK::ParallelMPICxx-MPI-TestProcess$"
    "^vtkFiltersParallelPython-testTransmit$"

    # Failures that appear related to OpenGL driver.
    #    MESA: error: ZINK: vkCreateInstance failed (VK_ERROR_INCOMPATIBLE_DRIVER)
    #    glx: failed to create drisw screen
    #    failed to load driver: zink
    "^VTK::RenderingCellGridPython-TestCellGridRendering$"
    )
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "offscreen")
  list(APPEND test_exclusions
    # Failed to open the display
    "^VTK::RenderingExternalCxx-TestGLUTRenderWindow$")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "windows")
  list(APPEND test_exclusions
    # Image size mismatches
    "^VTK::ChartsCoreCxx-TestMultipleScalarsToColors$"
    "^VTK::FiltersCorePython-TestOrientedFlyingEdgesPlaneCutter2$"
    "^VTK::RenderingOpenGL2Cxx-TestToneMappingPass$"

    # PATH manipulations needed
    "^VTKExample-ImageProcessing/Cxx$"
    "^VTKExample-IO/Cxx$"
    "^VTKExample-Medical/Cxx$"
    "^VTKExample-Modelling/Cxx$"
    "^VTKExample-Modules/UsingVTK$"
    "^VTKExample-Modules/Wrapping$"

    # Blank test image
    "^VTK::GUISupportQtCxx-TestQVTKOpenGLWindowWithDisabledInteractor$"

    # Timeouts; need investigation.
    "^VTK::GUISupportQtCxx-TestQVTKOpenGLStereoWidget$"
    "^VTK::GUISupportQtCxx-TestQVTKOpenGLStereoWidgetPicking$"
    "^VTK::GUISupportQtCxx-TestQVTKOpenGLStereoWidgetQWidgetWidget$"
    "^VTK::GUISupportQtCxx-TestQVTKOpenGLStereoWidgetWithChartHistogram2D$"
    "^VTK::GUISupportQtCxx-TestQVTKOpenGLStereoWidgetWithDisabledInteractor$"
    "^VTK::GUISupportQtCxx-TestQVTKOpenGLStereoWidgetWithMSAA$"
    "^VTK::GUISupportQtQuickCxx-TestQQuickVTKRenderItem$"
    "^VTK::GUISupportQtQuickCxx-TestQQuickVTKRenderItemWidget$"
    "^VTK::GUISupportQtQuickCxx-TestQQuickVTKRenderWindow$"

    # Flaky on windows for some reasons:
    # https://gitlab.kitware.com/vtk/vtk/-/issues/18640
    "^VTK::FiltersStatisticsCxx-TestMultiCorrelativeStatistics$"

    # Fail to present D3D resources (see #18657)
    "^VTK::RenderingOpenGL2Cxx-TestWin32OpenGLDXRenderWindow$"

    # https://gitlab.kitware.com/vtk/vtk/-/issues/19183
    "^VTK::RenderingCellGridPython-TestCellGridRendering$"
    # Thread-local objects are not destroyed at program exit.
    # https://stackoverflow.com/questions/50897768/in-visual-studio-thread-local-variables-destructor-not-called-when-used-with
    "^VTK::FiltersCellGridPython-TestCellGridPointProbe$"
    "^VTK::FiltersCellGridPython-TestCellGridRange$"
    "^VTK::FiltersCellGridPython-TestUnstructuredGridToCellGrid$"

    # https://gitlab.kitware.com/vtk/vtk/-/issues/19400
    "^VTK::RenderingCoreCxx-TestResizingWindowToImageFilter$"
  )
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "osmesa")
  list(APPEND test_exclusions
    # Flaky tests. They sometimes pass.
    "^VTK::InteractionWidgetsPython-TestInteractorEventRecorder$")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos_arm64")
  list(APPEND test_exclusions
    # Crowded geometry?
    # https://gitlab.kitware.com/vtk/vtk/-/issues/18230
    "^VTK::ViewsInfovisCxx-TestTreeMapView$"

    # Line rendering differences.
    # https://gitlab.kitware.com/vtk/vtk/-/issues/18229
    "^VTK::FiltersHyperTreeCxx-TestHyperTreeGridBinaryClipPlanes$"
    "^VTK::RenderingAnnotationCxx-TestCubeAxes3$"
    "^VTK::RenderingAnnotationCxx-TestCubeAxesWithYLines$")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos_x86_64")
  list(APPEND test_exclusions
    # Screenshot issue for test comparison with background buffer (intermittent)
    "^VTK::GUISupportQtQuickCxx-TestQQuickVTKRenderItemWidget$"
    "^VTK::GUISupportQtQuickCxx-TestQQuickVTKRenderWindow$"
    # MacOS OpenGL issue (intermittent)
    "^VTK::RenderingCellGridPython-TestCellGridRendering$"
    "^VTK::FiltersCellGridPython-TestUnstructuredGridToCellGrid$"
    
    # https://gitlab.kitware.com/vtk/vtk/-/issues/19372
    "^VTK::IOIOSSPython-TestIOSSCellGridReader$"
  )
endif ()

if (("$ENV{CMAKE_CONFIGURATION}" MATCHES "offscreen" AND "$ENV{CMAKE_CONFIGURATION}" MATCHES "ext_vtk") OR
    ("$ENV{CMAKE_CONFIGURATION}" MATCHES "linux" AND "$ENV{CMAKE_CONFIGURATION}" MATCHES "wheel" AND "$ENV{CMAKE_CONFIGURATION}" MATCHES "osmesa"))
  # These tests fail when using an external VTK because the condition
  # VTK_USE_X, vtk_can_do_onscreen AND NOT VTK_DEFAULT_RENDER_WINDOW_OFFSCREEN should be false
  # in Interaction/Style/Testing/Python/CMakeLists.txt
  list(APPEND test_exclusions
    "^VTK::InteractionStylePython-TestStyleJoystickActor$"
    "^VTK::InteractionStylePython-TestStyleJoystickCamera$"
    "^VTK::InteractionStylePython-TestStyleRubberBandZoomPerspective$"
    "^VTK::InteractionStylePython-TestStyleTrackballCamera$"
    "^VTK::RenderingCoreCxx-TestInteractorTimers$")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "linux" AND "$ENV{CMAKE_CONFIGURATION}" MATCHES "wheel" AND "$ENV{CMAKE_CONFIGURATION}" MATCHES "egl")
  # These tests fail when using an external VTK because the condition
  # VTK_USE_X, vtk_can_do_onscreen AND NOT VTK_DEFAULT_RENDER_WINDOW_OFFSCREEN should be false
  # in Interaction/Style/Testing/Python/CMakeLists.txt
  list(APPEND test_exclusions
    "^VTK::InteractionStylePython-TestStyleJoystickActor$"
    "^VTK::InteractionStylePython-TestStyleJoystickCamera$"
    "^VTK::InteractionStylePython-TestStyleRubberBandZoomPerspective$"
    "^VTK::InteractionStylePython-TestStyleTrackballCamera$"
    "^VTK::InteractionWidgetsPython-TestInteractorEventRecorder$")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "stdthread")
  list(APPEND test_exclusions
    # Test is flaky with STDThread
    # See #18555
    "^VTK::FiltersFlowPathsCxx-TestEvenlySpacedStreamlines2D$"
    )
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "wheel")
  list(APPEND test_exclusions
    # The wheels have a broken `proj.db`.
    # https://gitlab.kitware.com/vtk/vtk/-/issues/18750
    "^VTK::GeovisCorePython-TestGeoProjection$")

  if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "linux" OR
      "$ENV{CMAKE_CONFIGURATION}" MATCHES "osmesa")
    list(APPEND test_exclusions
      # Line rendering differences.
      # https://gitlab.kitware.com/vtk/vtk/-/issues/18098
      "^VTK::FiltersCorePython-contourCells$"
      "^VTK::FiltersGeneralPython-TestFEDiscreteClipper2D$"
      "^VTK::FiltersModelingPython-TestCookieCutter$"
      "^VTK::FiltersModelingPython-TestCookieCutter3$"
      "^VTK::FiltersModelingPython-TestImprintFilter3$"
      "^VTK::FiltersModelingPython-TestImprintFilter6$"
      "^VTK::FiltersSourcesPython-TestStaticCellLocatorLineIntersection$"
      "^VTK::InteractionWidgetsPython-TestTensorWidget2$"
      # Intermittent flakiness; may be related to CI runner OpenGL config.
      # Appears as a colormap or color-range failure:
      "^VTK::FiltersCellGridPython-TestUnstructuredGridToCellGrid$"
    )
  endif ()
  if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos.*_x86_64")
    list(APPEND test_exclusions
      # MacOS OpenGL issue (intermittent)
      "^VTK::FiltersCellGridPython-TestUnstructuredGridToCellGrid$"
      "^VTK::IOIOSSPython-TestIOSSCellGridReader$"
      "^VTK::RenderingCellGridPython-TestCellGridRendering$"
    )
  endif()
  if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "windows")
    # Windows wheels are intermittently flaky and not reproducible.
    list(APPEND test_exclusions
      "^VTK::RenderingCellGridPython-TestCellGridRendering$"
    )
  endif()
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "qt" AND
    NOT "$ENV{CMAKE_CONFIGURATION}" MATCHES "qt5")
  list(APPEND test_exclusions
    # Qt6 test failures that need investigation.
    # https://gitlab.kitware.com/vtk/vtk/-/issues/18946
    "^VTK::GUISupportQtCxx-TestQtDebugLeaksView$"
    "^VTK::GUISupportQtQuickCxx-TestQQuickVTKRenderItem$"
    "^VTK::GUISupportQtQuickCxx-TestQQuickVTKRenderItemWidget$"
    "^VTK::GUISupportQtQuickCxx-TestQQuickVTKRenderWindow$")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "ospray")
  list(APPEND test_exclusions
    # Fails with:
    #     ZINK: vkCreateInstance failed (VK_ERROR_INCOMPATIBLE_DRIVER)
    #     glx: failed to create drisw screen
    #     failed to load driver: zink
    "^VTK::RenderingCellGridPython-TestCellGridRendering$"
    # Cache segfaults on docker
    "^VTK::RenderingRayTracingCxx-TestOSPRayCache$"
    # https://github.com/ospray/ospray/issues/571
    "^VTK::RenderingRayTracingCxx-TestPathTracerMaterials$")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "^wasm(32|64)")
  list(APPEND test_exclusions
    # https://gitlab.kitware.com/vtk/vtk/-/issues/19343
    "^VTK::RenderingCoreCxx-TestActorLightingFlag$"
    "^VTK::RenderingCoreCxx-TestBlockOpacity$"
    "^VTK::RenderingCoreCxx-TestCompositeDataDisplayAttributes$"
    "^VTK::RenderingCoreCxx-TestCompositePolyDataMapperMixedGeometryEdges$"
    "^VTK::RenderingCoreCxx-TestCompositePolyDataMapperPartialFieldData$"
    "^VTK::RenderingCoreCxx-TestCompositePolyDataMapperSpheres$"
    "^VTK::RenderingCoreCxx-TestCompositePolyDataMapperVertices$"
    "^VTK::RenderingCoreCxx-TestEdgeFlags$"
    "^VTK::RenderingCoreCxx-TestFollowerPicking$"
    "^VTK::RenderingCoreCxx-TestGlyph3DMapperMasking$"
    "^VTK::RenderingCoreCxx-TestGlyph3DMapperOrientationArray$"
    "^VTK::RenderingCoreCxx-TestLabeledContourMapperWithActorMatrix$"
    "^VTK::RenderingCoreCxx-TestOpacity$"
    "^VTK::RenderingCoreCxx-TestPolyDataMapperNormals$"
    "^VTK::RenderingCoreCxx-TestRenderLinesAsTubes$"
    "^VTK::RenderingCoreCxx-TestTextureRGBADepthPeeling$"
    "^VTK::RenderingCoreCxx-TestTranslucentImageActorDepthPeeling$"
    "^VTK::RenderingCoreCxx-TestTranslucentLUTDepthPeeling$"
    "^VTK::RenderingCoreCxx-TestTranslucentLUTTextureDepthPeeling$"
    "^VTK::RenderingCoreCxx-TestResizingWindowToImageFilter$"
    "^VTK::RenderingCoreCxx-TestTextureWrap$"
    "^VTK::RenderingOpenGL2Cxx-TestCoincident$"
    "^VTK::RenderingOpenGL2Cxx-TestCompositeDataPointGaussian$"
    "^VTK::RenderingOpenGL2Cxx-TestCompositeDataPointGaussianSelection$"
    "^VTK::RenderingOpenGL2Cxx-TestCompositePolyDataMapper2CellScalars$"
    "^VTK::RenderingOpenGL2Cxx-TestCompositePolyDataMapper2MixedGeometryCellScalars$"
    "^VTK::RenderingOpenGL2Cxx-TestCompositePolyDataMapper2MixedGeometryEdges$"
    "^VTK::RenderingOpenGL2Cxx-TestCompositePolyDataMapper2PartialFieldData$"
    "^VTK::RenderingOpenGL2Cxx-TestCompositePolyDataMapper2Pickability$"
    "^VTK::RenderingOpenGL2Cxx-TestCompositePolyDataMapper2Picking$"
    "^VTK::RenderingOpenGL2Cxx-TestCompositePolyDataMapper2Spheres$"
    "^VTK::RenderingOpenGL2Cxx-TestCompositePolyDataMapper2Vertices$"
    "^VTK::RenderingOpenGL2Cxx-TestDepthPeelingOcclusionQuery$"
    "^VTK::RenderingOpenGL2Cxx-TestDepthPeelingPass$"
    "^VTK::RenderingOpenGL2Cxx-TestDepthPeelingPassViewport$"
    "^VTK::RenderingOpenGL2Cxx-TestEDLPass$"
    "^VTK::RenderingOpenGL2Cxx-TestFlipRenderFramebuffer$"
    "^VTK::RenderingOpenGL2Cxx-TestFluidMapper$"
    "^VTK::RenderingOpenGL2Cxx-TestFramebufferHDR$" # flaky
    "^VTK::RenderingOpenGL2Cxx-TestGaussianBlurPass$"
    "^VTK::RenderingOpenGL2Cxx-TestGlyph3DMapperEdges$"
    "^VTK::RenderingOpenGL2Cxx-TestHiddenLineRemovalPass$"
    "^VTK::RenderingOpenGL2Cxx-TestLightingMapNormalsPass$"
    "^VTK::RenderingOpenGL2Cxx-TestMultiTexturing$"
    "^VTK::RenderingOpenGL2Cxx-TestMultiTexturingInterpolateScalars$"
    "^VTK::RenderingOpenGL2Cxx-TestOutlineGlowPass$"
    "^VTK::RenderingOpenGL2Cxx-TestPBRClearCoat$"
    "^VTK::RenderingOpenGL2Cxx-TestPBREdgeTint$"
    "^VTK::RenderingOpenGL2Cxx-TestPBRHdrEnvironment$"
    "^VTK::RenderingOpenGL2Cxx-TestPointFillPass$"
    "^VTK::RenderingOpenGL2Cxx-TestPointGaussianMapper$"
    "^VTK::RenderingOpenGL2Cxx-TestPointGaussianMapperAnisotropic$"
    "^VTK::RenderingOpenGL2Cxx-TestPointGaussianMapperOpacity$"
    "^VTK::RenderingOpenGL2Cxx-TestPointGaussianSelection$"
    "^VTK::RenderingOpenGL2Cxx-TestProgramPointSize$"
    "^VTK::RenderingOpenGL2Cxx-TestRemoveActorNonCurrentContext$"
    "^VTK::RenderingOpenGL2Cxx-TestDirectSelectionRendering$"
    "^VTK::RenderingOpenGL2Cxx-TestSimpleMotionBlur$" # flaky
    "^VTK::RenderingOpenGL2Cxx-TestSpherePoints$"
    "^VTK::RenderingOpenGL2Cxx-TestSphereVertex$"
    "^VTK::RenderingOpenGL2Cxx-TestSSAOPass$" # shader error
    "^VTK::RenderingOpenGL2Cxx-TestSSAOPassWithRenderer$" # shader error
    "^VTK::RenderingOpenGL2Cxx-TestSurfaceInterpolationSwitch$"
    "^VTK::RenderingOpenGL2Cxx-TestTexture16Bits$"
    "^VTK::RenderingOpenGL2Cxx-TestTextureBufferEmulation$"
    "^VTK::RenderingOpenGL2Cxx-TestValuePassFloatingPoint$"
    "^VTK::RenderingOpenGL2Cxx-TestVBOPLYMapper$"
    "^VTK::RenderingOpenGL2Cxx-TestWindowBlits$")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "fedora39_webgpu")
  list(APPEND test_exclusions
    "^VTK::RenderingWebGPUCxx-TestComputeFrustumCulling") # Crashes randomly with mesa-vulkan-drivers
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "^wasm(32|64)_emscripten_linux_chrome_ext_vtk$")
  list(APPEND test_exclusions
    # Fails when chrome uses software accelerated webgl2 in linux.
    "^VTK::RenderingCoreCxx-TestGlyph3DMapperCompositeDisplayAttributeInheritance$"
    "^VTK::RenderingCoreCxx-TestTransformCoordinateUseDouble$")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "^wasm(32|64)_emscripten_windows_chrome_ext_vtk$")
  list(APPEND test_exclusions
    # ERR_UNSUPPORTED_ESM_URL_SCHEME: absolute paths must be valid file:// URLs. Received protocol 'c:'
    "^VTK::WebAssemblyJavaScript")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "^wasm64")
  list (APPEND test_exclusions
    # WebGPU tests on wasm64 fail 
    "^VTK::RenderingWebGPUCxx-TestActorFaceCullingProperty"
    "^VTK::RenderingWebGPUCxx-TestCellScalarMappedColors"
    "^VTK::RenderingWebGPUCxx-TestComputeDoublePipelineRenderBuffer"
    "^VTK::RenderingWebGPUCxx-TestComputeFrustumCulling"
    "^VTK::RenderingWebGPUCxx-TestComputeOcclusionCulling"
    "^VTK::RenderingWebGPUCxx-TestComputeOcclusionCullingResize"
    "^VTK::RenderingWebGPUCxx-TestComputeModifyPointColors"
    "^VTK::RenderingWebGPUCxx-TestComputeModifyCellColors"
    "^VTK::RenderingWebGPUCxx-TestComputePass"
    "^VTK::RenderingWebGPUCxx-TestComputePassChained"
    "^VTK::RenderingWebGPUCxx-TestComputePassUniform"
    "^VTK::RenderingWebGPUCxx-TestComputePipelineRelease"
    "^VTK::RenderingWebGPUCxx-TestComputePointCloudMapper"
    "^VTK::RenderingWebGPUCxx-TestComputePointCloudMapperColors"
    "^VTK::RenderingWebGPUCxx-TestComputePointCloudMapperDepth"
    "^VTK::RenderingWebGPUCxx-TestComputePointCloudMapperGeometry"
    "^VTK::RenderingWebGPUCxx-TestComputePointCloudMapperResize"
    "^VTK::RenderingWebGPUCxx-TestComputeTexture"
    "^VTK::RenderingWebGPUCxx-TestLineRendering"
    "^VTK::RenderingWebGPUCxx-TestLowPowerRenderWindow"
    "^VTK::RenderingWebGPUCxx-TestPointScalarMappedColors"
    "^VTK::RenderingWebGPUCxx-TestScalarModeToggle"
    "^VTK::RenderingWebGPUCxx-TestSurfacePlusEdges"
    "^VTK::RenderingWebGPUCxx-TestQuad"
    "^VTK::RenderingWebGPUCxx-TestQuadPointRep"
    "^VTK::RenderingWebGPUCxx-TestRenderWindowChangeDeviceLater"
    "^VTK::RenderingWebGPUCxx-TestVertexRendering"
    "^VTK::RenderingWebGPUCxx-TestWireframe"
    "^VTK::RenderingWebGPUCxx-TestPointRendering_1"
    "^VTK::RenderingWebGPUCxx-TestPointRendering_2"
    "^VTK::RenderingWebGPUCxx-TestPointRendering_3"
    "^VTK::RenderingWebGPUCxx-TestPointRendering_4"
    "^VTK::RenderingWebGPUCxx-TestMixedGeometry_1"
    "^VTK::RenderingWebGPUCxx-TestMixedGeometry_2"
    "^VTK::RenderingWebGPUCxx-TestMixedGeometry_3"
    "^VTK::RenderingWebGPUCxx-TestReadPixels")
endif ()


if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "windows" AND
    "$ENV{CMAKE_CONFIGURATION}" MATCHES "debug")
  # Timeouts from debug builds (even with 5 minute limits). See #19212
  list(APPEND test_exclusions
    "^TestLoggerDisableSignalHandler$"
    "^VTK::FiltersCoreCxx-TestFeatureEdges$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestGhostCellsGenerator$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestPResampleWithDataSet$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestPUniformGridGhostDataGenerator$"
    "^VTK::FiltersParallelDIY2Cxx-MPI-TestPUnstructuredGridGhostCellsGenerator$"
    "^VTK::FiltersParallelFlowPathsCxx-MPI-TestPParticleTracers$"
    "^VTK::FiltersParallelGeometryCxx-MPI-ParallelConnectivity4$"
    "^VTK::GUISupportQtCxx-TestQVTKTableModelAdapter$"
    "^VTK::IOCGNSReaderCxx-TestCGNSUnsteadyTemporalSolution$"
    "^VTK::ParallelCoreCxx-TestThreadedCallbackQueue$"
    "^VTK::RenderingVolumeCxx-TestGPURayCastLabelMapValidity$")
endif ()

string(REPLACE ";" "|" test_exclusions "${test_exclusions}")
if (test_exclusions)
  set(test_exclusions "(${test_exclusions})")
endif ()
