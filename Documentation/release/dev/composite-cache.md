A caching mechanism has been added to vtkCompositeDataPipeline to prevent
re-execution of a non-composite algorithm on blocks that have not changed since
last execution. The caching is enabled by default but can be switched of by
SetGlobalDataCachingEnabled. Note: this is an EXPERIMENTAL feature.
