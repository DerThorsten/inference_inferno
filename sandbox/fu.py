import inferno
import numpy
import vigra

nVar = 50
nLabels = 3
model = inferno.models.GeneralDiscreteGraphicalModel(nVar, nLabels)


####################################################
# EXPLICIT UNARY
####################################################
# add explicit unary functions
vals = numpy.random.rand(nVar,nLabels)*5
vtiRange = model.addExplicitValueTables( vals)
# add potts factors
vis = numpy.arange(nVar)
fiRange = model.addFactors(vtiRange, vis)

####################################################
# EXPLICIT 2-ORDER
####################################################
# add explicit unary functions

nSecondOrder = 10 * nVar 
vals = numpy.random.rand(nSecondOrder,nLabels,nLabels)*5
vtiRange = model.addExplicitValueTables(vals)
# add potts factors
vis = numpy.random.randint(nVar,size=[nSecondOrder,2])
fiRange = model.addFactors(vtiRange, vis)



#####################################################
## EXPLICIT 4-ORDER
#####################################################
n4Order =  nVar 
vals = numpy.random.rand(nSecondOrder,nLabels,nLabels,nLabels,nLabels)*3
vtiRange = model.addExplicitValueTables(vals)
# add potts factors
vis = numpy.random.randint(n4Order,size=[nSecondOrder,4])
fiRange = model.addFactors(vtiRange, vis)








print model.nVariables

verbVisitor = inferno.inference.verboseVisitor(model,1000,False)
with vigra.Timer("inf-c"):
    inf = inferno.inference.messagePassing(model)



with vigra.Timer("inf"):
    inf.infer(verbVisitor.visitor())

with vigra.Timer("conf"):
    c =  inf.conf()

print c[0:20]

