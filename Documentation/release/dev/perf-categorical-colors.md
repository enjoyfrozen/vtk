## Performance improvements to speedup coloring with indexed colors

Several improvements to the performance of coloring with indexed colors (sometimes
called categorical colors) have been made. Besides improvements to internal datastructures
used to speed up lookup, the mapping now can use multiple threads (via vtkSMPTools) if
compiled with appropriate SMP backend.
