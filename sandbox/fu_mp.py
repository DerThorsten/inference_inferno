import inferno
import numpy
import vigra

nVar = 640*48
nLabels = 10
model = inferno.models.GeneralDiscreteGraphicalModel(nVar, nLabels)


####################################################
# EXPLICIT UNARY
####################################################
# add explicit unary functions
vals = numpy.random.rand(nVar,nLabels)*0.5
vtiRange = model.addExplicitValueTables( vals)
# add potts factors
vis = numpy.arange(nVar)
fiRange = model.addFactors(vtiRange, vis)

####################################################
# EXPLICIT 2-ORDER
####################################################
# add explicit unary functions

nSecondOrder = 20 * nVar 
# add potts factors
vis = numpy.random.randint(nVar,size=[nSecondOrder,2])
vv = numpy.where(vis[:,0]!=vis[:,1])
vis = vis[vv[0],:]
nSecondOrder = vis.shape[0]
print "ss",numpy.sum(vis[:,0]==vis[:,1])

vals = numpy.random.rand(nSecondOrder,nLabels,nLabels)
vtiRange = model.addExplicitValueTables(vals)
fiRange = model.addFactors(vtiRange, numpy.array(vis))






Opt  = inferno.inference.InferenceOptions

opt = Opt()
opt['damping'] = 1.0;

opt2 = Opt()
opt2['subOpt'] = opt
print opt
print opt2


defOpt = inferno.inference.messagePassingOptions(model)

print "defOpt",defOpt

verbVisitor = inferno.inference.verboseVisitor(model,1,False)
inf = inferno.inference.messagePassing(model)



with vigra.Timer("inf"):
    inf.infer(verbVisitor.visitor())

with vigra.Timer("conf"):
    c =  inf.conf()

print c[0:20]

