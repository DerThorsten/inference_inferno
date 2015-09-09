import vigra
from vigra import graphs 
from functools import partial
import numpy
import pylab
import inferno
import ufd
import h5py

def normalizeRawFeat(rawFeat, minF, maxF):
    t = maxF-minF
    nFeat = rawFeat.copy()
    nFeat -= minF[None,:]
    nFeat[nFeat<0] = 0
    nFeat /= t[None,:]
    nFeat[nFeat>1.0] = 1.0
    return nFeat

def addNonLinearity(edgeFeatures):
    pow3 = edgeFeatures**3
    squares = edgeFeatures**2
    logOfSquares = numpy.log(squares+1.0)
    edgeFeatures = numpy.concatenate(
        [
            edgeFeatures,
            squares,
            logOfSquares,
            #pow3
        ]
        ,
        axis=1)
    return edgeFeatures


filepath = '/home/tbeier/Desktop/fuu.jpg'   # input image path
superpixelDiameter = 20  # super-pixel size
slicWeight = 30.0   
# load image and convert to LAB
img = vigra.impex.readImage(filepath)

# get super-pixels with slic on LAB image
imgLab = vigra.colors.transform_RGB2Lab(img)
labels, nseg = vigra.analysis.slicSuperpixels(imgLab, slicWeight,
                                              superpixelDiameter)


# get rag
labels = vigra.analysis.labelImage(labels)
labels -=1 
gridGraph = graphs.gridGraph(img.shape[0:2])
rag = graphs.regionAdjacencyGraph(gridGraph, labels)
uv = rag.uvIds()


try:
    f = h5py.File("labels.h5",'r')
    edgeLabels = f['data'][:]
    f.close()
except:
    edgeLabels = None

#edgeLabels = None
# get gt
gui = vigra.graphs.TinyEdgeLabelGui(rag=rag, img=img, edgeLabels = edgeLabels)
gui.startGui()
edgeLabels = gui.edgeLabels


f = h5py.File("labels.h5",'w')
f['data'] = edgeLabels 
f.close()


# compute features
log = vigra.filters.laplacianOfGaussian
stEv = vigra.filters.structureTensorEigenvalues
hessianEv = vigra.filters.hessianOfGaussianEigenvalues
gaussSmooth = vigra.filters.gaussianSmoothing
gausGradMag = vigra.filters.gaussianGradientMagnitude
filters = [
    [ (partial(log,scale=s),'log%f'%s) for s in (1.0,2.0,3.0,4.0) ],
    [ (partial(stEv,innerScale=s, outerScale=s*1.5),'stEv %f 1.5'%s) for s in (1.0,2.0,3.0) ],
    [ (partial(stEv,innerScale=s, outerScale=s*2.5),'stEv %f 2.5'%s) for s in (1.0,2.0,3.0) ],
    #[ (partial(hessianEv,scale=s),'hessianEv %f'%s) for s in (1.0,2.0,4.0) ],
    [ (partial(gaussSmooth,sigma=s),'gaussSmooth %f'%s) for s in (1.0,4.0) ],
    [ (partial(gausGradMag,sigma=s),'gausGradMag %f'%s) for s in (1.0,2.0,4.0) ]
]

filters = [item for sublist in filters for item in sublist]

allEdgeFeatures = []


raw = img
gg = rag.baseGraph


for i,fAndName in enumerate(filters):
    f,fName = fAndName
    print i,len(filters)
    res = f(raw).squeeze()
    if res.ndim == 2:
        res = res[:,:,None]

    

    for c  in range(res.shape[2]):
        resC = res[:,:,c]
        for ss in [None,2.25]:
            if ss is not None:
                resCS = gaussSmooth(resC,sigma=ss)
            else:
                resCS = resC
            
            if False:
                vigra.imshow(resCS[:,:])
                vigra.show()


            nodeFeatures = rag.accumulateNodeFeatures(resCS)
            uF = nodeFeatures[uv[:,0]]
            vF = nodeFeatures[uv[:,1]]

            #resCS[0:40,0:100] = 0 


            ggEdgeMap = graphs.implicitMeanEdgeMap(gg, resCS)
            edgeFeatures = rag.accumulateEdgeFeatures(ggEdgeMap)

            #ggEdgeMap = graphs.edgeFeaturesFromImage(gg, resCS)
            #edgeFeatures = rag.accumulateEdgeFeatures(ggEdgeMap,'mean')

            assert len(edgeFeatures) == rag.edgeNum
            assert uv.shape[0] == rag.edgeNum

            d = numpy.abs(uF-vF)
            s = uF+vF
            p = uF*vF

            de = s/2.0 - edgeFeatures
            ade =numpy.abs(de)

            adU = numpy.abs(uF - edgeFeatures)
            adV = numpy.abs(vF - edgeFeatures)
            tmp = numpy.concatenate([adU[:,None],adV[:,None]],axis=1)
            maxAduv = numpy.max(tmp,axis=1)
            minAduv = numpy.min(tmp,axis=1)
            dmima = maxAduv - minAduv


            if False:
                fImg = rag.showEdgeFeature(raw, edgeFeatures, returnImg=True,cmap='hot')
                fig = pylab.figure()
                fig.add_subplot(1,3, 0)
                pylab.imshow(raw,cmap='gray')
                fig.add_subplot(1, 3, 1)
                pylab.imshow(fImg)#,cmap=cm.Greys_r)
                fig.add_subplot(1, 3, 2)
                pylab.imshow(resCS,cmap='gray')
                pylab.title(fName)
                pylab.show()

            fl = [
                d[:,None],
                s[:,None],
                p[:,None],
                de[:,None],
                ade[:,None],
                edgeFeatures[:,None],
                maxAduv[:,None],
                minAduv[:,None],
                dmima[:,None]
            ]
            ef = numpy.concatenate(fl,axis=1)
            allEdgeFeatures.append(ef)

edgeFeatures = numpy.concatenate(allEdgeFeatures,axis=1)
#edgeFeatures = addNonLinearity(edgeFeatures)
 
print edgeFeatures.shape

qs= [0.0001*100.0, 0.9999*100.0]
psarray = numpy.percentile(edgeFeatures,q=qs,axis=0)#,interpolation='nearest')
minVals = psarray[0]#,:]
maxVals = psarray[1]#,:]


edgeFeatures = normalizeRawFeat(edgeFeatures, minVals, maxVals).astype('float64')
edgesWithGt = numpy.where(edgeLabels != 0)


print edgesWithGt[0].size

X = []
Y = []

for ee in edgesWithGt[0]:
    x = edgeFeatures[ee,:]
    X.append(x)
    y = edgeLabels[ee]
    y += 1
    y /= 2
    Y.append(y)
X = numpy.array(X).astype('float32')
Y = numpy.array(Y)[:,None].astype('uint32')


rf = vigra.learning.RandomForest(treeCount=100, sample_classes_individually=True)
print X.shape, Y.shape
oob = rf.learnRF(X,Y)
print "OOB",oob


print "PREDICT"
X = edgeFeatures.astype('float32')
probs = rf.predictProbabilities(X)[:,1]
probs = probs[:,None]
eprobs = (probs + 0.001)/1.001
featP = edgeFeatures.copy()
featP *= eprobs
edgeFeatures = [
    edgeFeatures,
    featP,
    probs,
    (probs)**2.0,
    ((probs + 1)/2.0)**2.0
]
edgeFeatures = numpy.concatenate(edgeFeatures,axis=1).astype('float64')




ParaMcModel = inferno.models.ParametrizedMulticutModel

nFeatures = edgeFeatures.shape[1]
weightVector = inferno.learning.WeightVector(nFeatures, 0.0)

mVec = ParaMcModel.modelVector(1)
vis = ParaMcModel.lossFunctionVector('variationOfInformation',1)

gts = ParaMcModel.confMapVector(1)


edges = rag.uvIds().astype('uint64')
nVar = rag.nodeNum
print "edges.max()",edges.max(), "rag node num ", rag.nodeNum
assert edges.min() == 0
assert edges.max() == rag.nodeNum -1


print "assign"

model = mVec[0]
vi = vis[0]
gt = gts[0]


model._assign(nVar, edges, edgeFeatures, weightVector)
gt.assign(model,1)

print "sizes"
sizeMap = model.variableMap('float64', 1.0)
sizeMap.view()[:] = 1
vi.assign(model, sizeMap, useIgnoreLabel=True, ignoreLabel=-1,beta=0.5)

#vi.factorConf().assign(model)
print "fac conf"
vi.factorConf().view()[:] = edgeLabels[:]
vi.setUseHack(True)

nWeights = len(weightVector)
fixedWeightIndex = nWeights -1
weightVector[fixedWeightIndex] = 1.0


rType = inferno.learning.L2
regularizer = inferno.learning.Regularizer(rType, c=1.0 )

print "dset"
dset = inferno.learning.dataset.vectorDataset(mVec, vis, gts,regularizer=regularizer)
dset.weightConstraints().addBound(fixedWeightIndex, lowerBound=1.0, upperBound=1.0)

factory = inferno.inference.multicutFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)',numThreads=1)

print "dset"
sg = inferno.learning.learners.stochasticGradient
learner = sg(dset, maxIterations=20, nPertubations=5, sigma=2.1, seed=42,
                   n=60.0)

print "learn"
learner.learn(factory, weightVector)




for e in range(rag.edgeNum):
    if edgeLabels[e] == -1:
        model.changeBeta(e, 9999999.9)
    elif edgeLabels[e] == 1:
        model.changeBeta(e, -9999999.9)

solver = factory.create(model)

verboseVisitor = inferno.inference.verboseVisitor(model)
solver.infer(verboseVisitor.visitor())
conf = solver.conf()

arg = conf.view().astype('uint32')

rag.show(img, arg)
vigra.show()