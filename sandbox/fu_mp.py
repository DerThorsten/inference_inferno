import inferno
import numpy
import vigra

numpy.random.seed(7)

nVar = 100*25
nLabels = 10
model = inferno.models.GeneralDiscreteGraphicalModel(nVar, nLabels)


####################################################
# EXPLICIT UNARY
####################################################
# add explicit unary functions
vals = numpy.random.rand(nVar,nLabels)
vtiRange = model.addExplicitValueTables( vals)
# add potts factors
vis = numpy.arange(nVar)
fiRange = model.addFactors(vtiRange, vis)


####################################################
# L1 2-ORDER
####################################################
# add l1 2-order functions
if False:
    nSecondOrder = 50 * nVar 
    # add potts factors
    vis = numpy.random.randint(nVar,size=[nSecondOrder,2])
    vv = numpy.where(vis[:,0]!=vis[:,1])
    vis = vis[vv[0],:]
    nSecondOrder = vis.shape[0]


    vals = (numpy.random.rand(nSecondOrder)+0.5)
    vtiRange = model.addL1ValueTables(nLabels, vals)
    fiRange = model.addFactors(vtiRange, numpy.array(vis))

####################################################
# Trunc. L1 2-ORDER
####################################################
# add truncate l1 2-order functions
if False:
    nSecondOrder = 5 * nVar 
    # add potts factors
    vis = numpy.random.randint(nVar,size=[nSecondOrder,2])
    vv = numpy.where(vis[:,0]!=vis[:,1])
    vis = vis[vv[0],:]
    nSecondOrder = vis.shape[0]


    beta = (numpy.random.rand(nSecondOrder)+0.5)
    trunc = (numpy.random.rand(nSecondOrder))*nLabels
    vtiRange = model.addTruncatedL1ValueTables(nLabels, beta, trunc)
    fiRange = model.addFactors(vtiRange, numpy.array(vis))

####################################################
# Potts 2-ORDER
####################################################
# add explicit 2-order functions
if False:
    nSecondOrder =  nVar 
    # add potts factors
    vis = numpy.random.randint(nVar,size=[nSecondOrder,2])
    vv = numpy.where(vis[:,0]!=vis[:,1])
    vis = vis[vv[0],:]
    nSecondOrder = vis.shape[0]


    vals = (numpy.random.rand(nSecondOrder)-0.5)*0.01
    vtiRange = model.addPottsValueTables(nLabels, vals)
    fiRange = model.addFactors(vtiRange, numpy.array(vis))

####################################################
# EXPLICIT 2-ORDER
####################################################
if False:
    nSecondOrder = nVar 
    # add potts factors
    vis = numpy.random.randint(nVar,size=[nSecondOrder,3])
    vv = numpy.where(vis[:,0]!=vis[:,1])
    vis = vis[vv[0],:]
    nSecondOrder = vis.shape[0]
    
    
    vals = numpy.random.rand(nSecondOrder,nLabels,nLabels,nLabels)*0.01
    vtiRange = model.addExplicitValueTables(vals)
    fiRange = model.addFactors(vtiRange, numpy.array(vis))



####################################################
# EXPLICIT 3-ORDER
####################################################
if True:
    nSecondOrder = 50*nVar 
    # add potts factors
    vis = numpy.random.randint(nVar,size=[nSecondOrder,3])
    vv = numpy.where(vis[:,0]!=vis[:,1])
    vis = vis[vv[0],:]
    vv = numpy.where(vis[:,1]!=vis[:,2])
    vis = vis[vv[0],:]
    nSecondOrder = vis.shape[0]
    
    
    vals = numpy.random.rand(nSecondOrder,nLabels,nLabels,nLabels)*0.1
    vtiRange = model.addExplicitValueTables(vals)
    fiRange = model.addFactors(vtiRange, numpy.array(vis))






opts = inferno.inference.messagePassingOptions(model)
opts['damping'] = 0.2
opts['nSteps'] = long(10)
opts['nThreads'] = 0
opts['eps'] =  1.0e-09
opts['concurrency'] = 1
print opts

verbVisitor = inferno.inference.verboseVisitor(model,1,True)
inf = inferno.inference.messagePassing(model, opts, True)



with vigra.Timer("inf"):
    inf.infer(verbVisitor.visitor())
    print "inf is done"

with vigra.Timer("conf"):
    c =  inf.conf()

print c[0:20]

