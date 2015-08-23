from loss_functions import *



def variationOfInformation(model, sizeMap, 
                           useIgnoreLabel=False, 
                           ignoreLabel=-1):
    f = loss_functions._variationOfInformation
    return f(model, sizeMap, bool(useIgnoreLabel), long(ignoreLabel))
