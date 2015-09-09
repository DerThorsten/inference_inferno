from models import *
from .. import learning
import numpy




def injectorClass(clsToExtend):
    class InjectorClass(object):
        class __metaclass__(clsToExtend.__class__):
            def __init__(self, name, bases, dict):
                for b in bases:
                    if type(b) not in (self, type):
                        for k,v in dict.items():
                            setattr(b,k,v)
                tmp = type.__init__(self, name, bases, dict)
    return InjectorClass


def _extendMapCls(modelCls, mapCls):

    class moreNumpyVariableMap(injectorClass(mapCls), mapCls):

        def astype(self, dtype):

            retArray =  self.graph().variableMap(dtype=dtype)
            retView = retArray.view()
            view = self.view()
            retView[:] = view[:]
            return retView

        def __array__(self):
            return self.view()

        def __repr__(self):
            return repr(self.view())

        def __str__(self):
            return str(self.view())

        @property
        def ndim(self):
            return self.view().ndim

        @property
        def shape(self):
            return self.view().shape


        
def _extendModelClass(modelCls, classStr):
    ##inject some methods in the point foo
    class moreModel(injectorClass(modelCls), modelCls):

        def factorMap(self, dtype, initValue=None):
            ownName = self.__class__.__name__
            mapname = ownName + "FactorMap_" + str(numpy.dtype(dtype))
            mapCls = models.__dict__[mapname]
            if initValue is None:
                return mapCls(self)
            else:
                return mapCls(self, initValue)
        def unaryMap(self, dtype, initValue=None):
            ownName = self.__class__.__name__
            mapname = ownName + "UnaryMap" + str(numpy.dtype(dtype))
            mapCls = models.__dict__[mapname]
            if initValue is None:
                return mapCls(self)
            else:
                return mapCls(self, initValue)
        def variableMap(self, dtype, initValue=None):
            ownName = self.__class__.__name__
            mapname = ownName + "VariableMap_" + str(numpy.dtype(dtype))
            mapCls = models.__dict__[mapname]
            if initValue is None:
                return mapCls(self)
            else:
                return mapCls(self, initValue)

        def confMap(self):
            return self.variableMap(dtype='int64')

        @classmethod
        def learningDataset(cls, lossFunction, nModels=0):
            dsetCls = None
            if lossFunction == 'variationOfInformation':
                dsetClsStr = "VectorDataset"+classStr+'VariationOfInformation'
                dsetCls = learning.dataset.__dict__[dsetClsStr]
            else:
                raise RuntimeError("currently only 'variationOfInformation' is allowed as loss")
            if dsetCls is not None:
                return dsetCls(long(nModels))
            else:
                raise RuntimeError("cannot create dataset")

        @classmethod
        def modelVector(cls, size):
            return cls.modelVectorCls(size)

        @classmethod
        def confMapVector(cls, size):
            return cls.confMapVectorCls(size)

        @classmethod
        def lossFunctionVector(cls, lossName, size):
            ownName = cls.__name__
            lossClsName = None
            allowedLossNames = ['variationOfInformation','edgeHamming']
            if lossName == allowedLossNames[0]:
                lossClsName = 'VariationOfInformation'
            elif lossName == allowedLossNames[1]:
                lossClsName = 'EdgeHamming'
            else:
                raise RuntimeError('lossName must be in %s'%str(allowedLossNames))
                
            lossVecClsName = lossClsName + ownName + "Vector"
            lossVecCls = learning.loss_functions.__dict__[lossVecClsName]

            return lossVecCls(size)


        @classmethod
        def lossFunctionVector2(cls, lossName, size):
            assert lossName in ['partitionFScore', 'variationOfInformation2','randIndex']

            if lossName in ['partitionFScore', 'variationOfInformation2','randIndex']:
                vecClsName = '%sNonDecomposableLossFunctionBaseVector'%cls.__name__
                vecCls = learning.loss_functions.__dict__[vecClsName]
            return vecCls(size)

        @classmethod
        def lossAugmentedModelClass(cls, lossName):
            ownName = cls.__name__
            lossClsName = None
            allowedLossNames = ['edgeHamming']
            if lossName == allowedLossNames[0]:
                lossClsName = 'EdgeHamming'
            else:
                raise RuntimeError('lossName must be in %s'%str(allowedLossNames))
            
            lossAugmentedModelClsName = lossClsName + "LossAugmented" + ownName
            return  models.__dict__[lossAugmentedModelClsName]


def _extendModels():
    rawModelClasses = [
        "GeneralDiscreteModel", 
        "ParametrizedMulticutModel",
    ]



    for rawClsStr in rawModelClasses:

        rawCls = models.__dict__[rawClsStr]
        modelVectorCls = models.__dict__[rawClsStr+"Vector"]
        confMapVectorCls = models.__dict__[rawClsStr+"ConfMapVector"]

        rawCls.modelVectorCls = modelVectorCls
        rawCls.confMapVectorCls = confMapVectorCls
        # inject more functionality into model class
        _extendModelClass(rawCls,  rawClsStr)


_extendModels()
del _extendModels
del _extendModelClass


def parametrizedMulticutModel(nVar, edges, features, weightVector):
    nVar = long(nVar)
    edges = numpy.require(edges,dtype='uint64')
    features = numpy.require(features, dtype='float64')

    return models._parametrizedMulticutModel(nVar, edges, features, weightVector)
