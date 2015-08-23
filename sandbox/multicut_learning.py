import inferno
from inferno import models 
from inferno import learning 
from inferno.learning import loss_functions
import numpy



nFeatures = 3
weightVector = learning.WeightVector(nFeatures, 0.0)


dset = models.ParametrizedMulticutModel.learningDataset(lossFunction='variationOfInformation',
                                                        nModels=2)




model0 = dset.model(0)

nVar = 3
edges = numpy.require([[0,1],[1,2]], dtype='uint64')
features = numpy.require([[0,0,0],[0,0,0]], dtype='float64')

model0._assign(nVar, edges, features, weightVector)



gt = model0.variableMap('uint64').idMap
gt[0] = 0
gt[1] = 1
gt[2] = 1


sizeMap = model0.variableMap('float64').idMap
sizeMap[0] = 1
sizeMap[1] = 1
sizeMap[2] = 1
sizeMap = sizeMap.variableMap

dset.lossFunction(0).assign(sizeMap, False, long(-1))
dset.groundTruth(0) = gt.variableMap
