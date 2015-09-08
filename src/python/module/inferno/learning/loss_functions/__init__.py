from loss_functions import *
from enum import Enum



def variationOfInformation(model, sizeMap, 
                           useIgnoreLabel=False, 
                           ignoreLabel=-1):
    """
        deprecated
    """
    f = loss_functions._variationOfInformation
    return f(model, sizeMap, bool(useIgnoreLabel), long(ignoreLabel))
