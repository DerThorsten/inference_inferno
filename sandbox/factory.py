import inferno
import numpy
import vigra

numpy.random.seed(7)

nVar = 10
nLabels = 2
model = inferno.models.GeneralDiscreteModel(nVar, nLabels)


####################################################
# EXPLICIT UNARY
####################################################
# add explicit unary functions
vals = numpy.random.rand(nVar,nLabels)
vtiRange = model.addExplicitValueTables( vals)
# add potts factors
vis = numpy.arange(nVar)
fiRange = model.addFactors(vtiRange, vis)




nSecondOrder =  nVar*40
# add potts factors
vis = numpy.random.randint(nVar,size=[nSecondOrder,2])
vv = numpy.where(vis[:,0]!=vis[:,1])
vis = vis[vv[0],:]
nSecondOrder = vis.shape[0]


vals = (numpy.random.rand(nSecondOrder)-0.5)*0.1
print vals.min(),vals.max()
vtiRange = model.addPottsValueTables(nLabels, vals)
fiRange = model.addFactors(vtiRange, numpy.array(vis))





inf = inferno.inference.qpbo(model, useProbing=False)


verbVisitor = inferno.inference.verboseVisitor(model,1,False)
inf.infer(verbVisitor.visitor())

conf = inf.conf()
print conf
