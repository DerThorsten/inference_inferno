import inferno
import numpy
import vigra

numpy.random.seed(42)

nVar = 50
nSemanticClasses = 3
model = inferno.models.ModifiedMultiwaycutModel(nVar, nSemanticClasses)


# unaries (semantic costs)
vals = numpy.random.rand(nVar,nSemanticClasses)*5
vis = numpy.arange(nVar)


model.addUnaryFactors(vals, vis)

# potts (edge weights)
nSecondOrder = 10 * nVar 
vis = numpy.random.randint(nVar,size=[nSecondOrder,2])
vv = numpy.where(vis[:,0]!=vis[:,1])
vis = vis[vv[0],:]
nSecondOrder = vis.shape[0]
vals = numpy.random.rand(nSecondOrder)-0.5
fiRange = model.addPottsFactors(vals, vis)



inf = inferno.inference.hmmwc(model=model)


print type(inf)

verbVisitor = inferno.inference.verboseVisitor(model,1,False)
print inf.name()
inf.infer(verbVisitor.visitor())