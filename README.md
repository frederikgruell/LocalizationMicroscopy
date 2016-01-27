=============================
Stochastic Localization Microscopy
=============================

Image stack analysis for Spectral Position Determination Microscopy (SPDM) with MaxJ on FPGAs


Introduction
------------

This implementation runs analysis for Localization Microscopy on a DFE. The image data must be provided as a TIFF image stack. The result is a 
list of locations, including the total intensity, the location width and error. See the paper in the DOCS directory for a description of the algorithm.


Usage
-----

Run the Runrule for simulation or hardware, pass the image stack as the first argument to the executable and write the output into an empty *.tsv file. An example image stack is provided in the DOCS directory. You can use the simple image viewer provided in this github repository to render the super-resolution image from the output.


