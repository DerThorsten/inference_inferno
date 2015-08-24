import inferno
from inferno import models 
from inferno import learning 
from inferno.learning import loss_functions
from inferno.learning import learners
import numpy



ParaMcModel = models.ParametrizedMulticutModel



def makeDset():
    nFeatures = 3
    nVar = 4
    weightVector = learning.WeightVector(nFeatures, 0.0)
    edges = numpy.require([[0,1],[1,3],[0,2],[2,3]], dtype='uint64')
    features = [
                    [0,0,1], # cut edge
                    [0,0,1],
                    [0,0,1],
                    [0,1,1]  # cut edge
    ]
    features = numpy.require(features, dtype='float64')

    ParaMcModel = models.ParametrizedMulticutModel
    dset = ParaMcModel.learningDataset(lossFunction='variationOfInformation',
                                       nModels=2)

    for i in range(2):
        # assign model
        model = dset.model(i)
        model._assign(nVar, edges, features, weightVector)

        # assign loss function
        lossFunction = dset.lossFunction(i)
        sizeMap = model.variableMap('float64', 1.0)
        lossFunction.assign(model, sizeMap)

        # make gt
        gt = dset.groundTruth(i)
        gt.assign(model)
        gt.idMap[0] = 0
        gt.idMap[1] = 1
        gt.idMap[2] = 0
        gt.idMap[3] = 1

    return dset, weightVector

dset, weightVector = makeDset()


# make the learner
learner = learners.stochasticGradient(dset, nPertubations=5)


factory = inferno.inference.multicutFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)')

# do the learning
learner.learn(factory, weightVector)