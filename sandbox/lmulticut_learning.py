import inferno
from inferno import models 
from inferno import learning 
from inferno.learning import loss_functions
from inferno.learning import learners
from inferno.learning import dataset
import numpy



ParaMcModel = models.ParametrizedMulticutModel



def makeDset():
    nFeatures = 3
    nVar = 9
    weightVector = learning.WeightVector(nFeatures, 0.0)

    edges =[
        [0,1],  #cut
        [1,2],
        [3,4],  #cut
        [4,5],
        [6,7],  #cut
        [7,8],
        [0,3],  #cut
        [1,4],  #cut
        [2,5],  #cut
        [3,6],   
        [4,7],
        [5,8]
    ]
    edges = numpy.require(edges, dtype='uint64')
    features = [
                 [ 0.0,  1.0, 1.0],  # (0,1)  #cut
                 [-1.0,  0.0, 1.0],  # (1,2)      
                 [ 0.0,  1.0, 1.0],  # (3,4)  #cut
                 [-1.0,  0.0, 1.0],  # (4,5)      
                 [ 0.0,  1.0, 1.0],  # (6,7)  #cut
                 [-1.0,  0.0, 1.0],  # (7,8)   
                 [-1.0,  1.0, 1.0],  # (0,3)  #cut
                 [ 1.0, -1.0, 1.0],  # (1,4)  #cut
                 [ 0.0,  1.0, 1.0],  # (2,5)  #cut
                 [-1.0,  0.0, 1.0],  # (3,6)          
                 [-1.0,  0.0, 1.0],  # (4,7)      
                 [-1.0,  0.0, 1.0]   # (5,8)      
    ]
    features = numpy.require(features, dtype='float64')

    ParaMcModel = models.ParametrizedMulticutModel


    mVec = ParaMcModel.modelVector(2)
    lossFunctions = ParaMcModel.lossFunctionVector('variationOfInformation',2)
    gts = ParaMcModel.confMapVector(2)



    for i, (model ,lossFunction, gt) in enumerate(zip(mVec,lossFunctions,gts)):

        # assign model
        model._assign(nVar, edges, features, weightVector)


        # assign loss function
        sizeMap = model.variableMap('float64', 1.0)
        lossFunction.assign(model, sizeMap)

        # make gt
        gt.assign(model)
        gt.idMap[0] = 0
        gt.idMap[1] = 1
        gt.idMap[2] = 1
        gt.idMap[3] = 3
        gt.idMap[4] = 4
        gt.idMap[5] = 4
        gt.idMap[6] = 3
        gt.idMap[7] = 4
        gt.idMap[8] = 4


    return (mVec,lossFunctions,gts), weightVector

(mVec,lossFunctions,gts), weightVector = makeDset()




dset = dataset.vectorDataset(mVec, lossFunctions, gts)
# make the learner
learner = learners.stochasticGradient(dset, nPertubations=200, alpha=0.99,
                                      sigma=30.0)


factory = inferno.inference.multicutFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)')

# do the learning
learner.learn(factory, weightVector)
