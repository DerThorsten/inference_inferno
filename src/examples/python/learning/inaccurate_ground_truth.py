import vigra
import inferno
import numpy

# <markdowncell>
#
#
#
#
#
nModels = 2
shape = [21,21]
noiseLevel = 1.0


for i in range(nModels):

    gt = numpy.zeros(shape)
    gt[10,:] = 1
    gt[:,10] = 1

    data = numpy.zeros(shape,dtype='float32')
    data[8,:] = 1
    data[:,7] = 1
    data += noiseLevel*numpy.random.rand(*shape)

    #data = vigra.gaussianSmoothing(data, 0.75)

    vigra.imshow(data)
    vigra.show()
