Muser
=====

The algorithm for converting a 3D mesh into an audio file relies
firstly on having a **buffer** of values that can be generated 
from the mesh and can then be written to a ``.wav`` file. 

The buffer
----------

The buffer that we use is a 1-dimensional vector of doubles, but conceptually we are handling
it as a 2-dimensional vector. Given that a 3D mesh has UV texture
coordinates associated with each vertex, we can imagine our buffer
as a spectrogram of sorts, where the rows of the 2D vector signify
an amount of hertz, ascending from bottom to top, and the columns 
signify the sum all of the sine waves of varying Hz and their magnitudes
that together make up the combined wave value for that specific audio sample.


