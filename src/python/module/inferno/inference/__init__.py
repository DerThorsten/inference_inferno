from inference import *
from ..models import modelClasses
import string
from functools import partial
from inspect import isclass



def infFactory(modelOrClass,solverName, *args, **kwargs):
    if isclass(modelOrClass):
        classOfModel = modelOrClass
    else :
        classOfModel = modelOrClass.__class__
    cName = classOfModel.__name__

    factory = inference.__dict__['%sFactory'%solverName + cName]
    return factory(*args, **kwargs)

def infFactoryAndCreate(model,solverName, *args, **kwargs):
    classOfModel = model.__class__
    cName = classOfModel.__name__
    factory = inference.__dict__['%sFactory'%solverName + cName]
    return factory(*args, **kwargs).create(model)


qpboFactory = partial(infFactory,solverName='qpbo')
qpbo = partial(infFactoryAndCreate,solverName='qpbo')

hmmwcFactory = partial(infFactory,solverName='hmmwc')
hmmwc = partial(infFactoryAndCreate,solverName='hmmwc')

del infFactory
del infFactoryAndCreate
