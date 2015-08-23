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

def _extendModelsVariableMapClass(varMapCls):
        ##inject some methods in the point foo
        class moreVarMap(injectorClass(varMapCls), varMapCls):

            class IdMap(object):
                def __init__(self, variableMap):
                    self.variableMap = variableMap
                def __repr__(self):
                    vm = self.variableMap
                    return repr(vm.asIdIndexableArray(vm.model()))
                def __str__(self):
                    vm = self.variableMap
                    return str(vm.asIdIndexableArray(vm.model()))
                def __getitem__(self, key):
                    vm = self.variableMap
                    return vm.readAtId(vm.model(), long(key))
                def __setitem__(self, key, value):
                    vm = self.variableMap
                    return vm.writeAtId(vm.model(), long(key), value)
        
                def model(self):
                    return self.variableMap.model

            def __repr__(self):
                return "i am a var map"

            @property              
            def idMap(self):
                return moreVarMap.IdMap(self)

            @property              
            def descMap(self):
                return moreVarMap.IdMap(self)

def _extendModelClass(modelCls):
    ##inject some methods in the point foo
    class moreModel(injectorClass(modelCls), modelCls):
        def __repr__(self):
            return 'hi i am a gm'


        def variableMap(self,dtype):
            varMap = self.__class__.variableMapClsDict[dtype](self)
            return varMap

        def confMap(self):
            return self.variableMap(dtype='int64')

        @classmethod
        def learningDataset(cls, lossFunction, nModels=0):
            dsetCls = None
            if lossFunction == 'variationOfInformation':
                dsetClsStr = "VectorDataset"+rawClsStr+'VariationOfInformation'
                dsetCls = learning.dataset.__dict__[dsetClsStr]
            else:
                raise RuntimeError("currently only 'variationOfInformation' is allowed as loss")
            if dsetCls is not None:
                return dsetCls(long(nModels))
            else:
                raise RuntimeError("cannot create dataset")


def _extendModels():
    rawModelClasses = [
        "GeneralDiscreteModel", 
        "ParametrizedMulticutModel",
    ]

    variableMapDtypeStrs = {
        'bool':'Bool',
        'uint32':'UInt32',
        'uint64':'UInt64',
        'int64':'Int64',
        'int32':'Int32',
        'float64':'Float64',
        'float32':'Float32'
    }




    for rawClsStr in rawModelClasses:

        rawCls = models.__dict__[rawClsStr]

        rawCls.variableMapClsDict =  dict()
        for key in variableMapDtypeStrs.keys():
            cppDtypeName = variableMapDtypeStrs[key]
            varMapCls =  models.__dict__[rawClsStr+"VariableMap"+cppDtypeName]
            rawCls.variableMapClsDict[key] = varMapCls

            # inject more functionality into varMap classe
            _extendModelsVariableMapClass(varMapCls)

        # inject more functionality into model class
        _extendModelClass(rawCls)


_extendModels()
del _extendModels
del _extendModelsVariableMaps
del _extendModelClass


def parametrizedMulticutModel(nVar, edges, features, weightVector):
    nVar = long(nVar)
    edges = numpy.require(edges,dtype='uint64')
    features = numpy.require(features, dtype='float64')

    return models._parametrizedMulticutModel(nVar, edges, features, weightVector)
