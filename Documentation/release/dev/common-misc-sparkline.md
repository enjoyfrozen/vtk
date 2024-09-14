## Printable Sparklines

The CommonMisc module now provides a vtkSparkline class
you can use to generate a UTF-8 encoded bar chart for
printing a diagram of an array.

For example, a vtkDoubleArray holding `{0.0, 2.0, 1.5., 1.0}`
will yield "▁█▆▄". You can set a maximum or fixed width for
the sparkline and the array will be sampled as needed.
