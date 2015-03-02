import inferno
import numpy
import vigra

numpy.random.seed(7)

nVar = 100*10
nLabels = 100
model = inferno.models.GeneralDiscreteGraphicalModel(nVar, nLabels)


####################################################
# EXPLICIT UNARY
####################################################
# add explicit unary functions
vals = numpy.random.rand(nVar,nLabels)*5.0
vtiRange = model.addExplicitValueTables( vals)
# add potts factors
vis = numpy.arange(nVar)
fiRange = model.addFactors(vtiRange, vis)

####################################################
# Potts 2-ORDER
####################################################
# add explicit 2-order functions
nSecondOrder = 50 * nVar 
# add potts factors
vis = numpy.random.randint(nVar,size=[nSecondOrder,2])
vv = numpy.where(vis[:,0]!=vis[:,1])
vis = vis[vv[0],:]
nSecondOrder = vis.shape[0]


vals = (numpy.random.rand(nSecondOrder)-0.5)
vtiRange = model.addPottsValueTables(nLabels, vals)
fiRange = model.addFactors(vtiRange, numpy.array(vis))




####################################################
# EXPLICIT 2-ORDER
####################################################
# add explicit 2-order functions
#nSecondOrder = 10 * nVar 
## add potts factors
#vis = numpy.random.randint(nVar,size=[nSecondOrder,2])
#vv = numpy.where(vis[:,0]!=vis[:,1])
#vis = vis[vv[0],:]
#nSecondOrder = vis.shape[0]
#
#
#vals = numpy.random.rand(nSecondOrder,nLabels,nLabels)
#vtiRange = model.addExplicitValueTables(vals)
#fiRange = model.addFactors(vtiRange, numpy.array(vis))






opts = inferno.inference.messagePassingOptions(model)
opts['damping'] = 0.5
opts['nSteps'] = 20

print opts

verbVisitor = inferno.inference.verboseVisitor(model,1,False)
inf = inferno.inference.messagePassing(model, opts)



with vigra.Timer("inf"):
    inf.infer(verbVisitor.visitor())

with vigra.Timer("conf"):
    c =  inf.conf()

print c[0:20]

