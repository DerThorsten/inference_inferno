import sys
import os
import numpy
import h5py
import vigra

import make_ds
import inferno
import time





def extract2d(neuroproofRoot, stride = 5, maxSamples = 50):
    p  = neuroproofRoot + "training_sample2/"

    pGtStack =  p + "groundtruth.h5"
    pSpStack =  p + "oversegmented_stack_labels.h5"
    pRaw = p + "grayscale_maps/"

    f = h5py.File(pGtStack,'r')
    dset = f['stack']
    gtStack = dset[:,:,:].astype('uint32')
    f.close()

    f = h5py.File(pSpStack,'r')
    dset = f['stack']
    spStack = dset[:,:,:].astype('uint32')
    f.close()

    raws = []
    osegs = []
    gts = []

    c = 0 
    for i in range(0,gtStack.shape[0],stride):
        
        ii = 2600 + i

        pRawFile = pRaw + "iso.%d.png"%ii
        raw = vigra.impex.readImage(pRawFile).squeeze()
        #raw = raw.T
        raws.append(raw)
        gts.append(vigra.analysis.labelImage(gtStack[i,:,:].T)-1)
        sp2d = vigra.analysis.labelImage(spStack[i,:,:].T)-1
        print sp2d.shape
        osegs.append(sp2d)

        #vigra.segShow(raw,osegs[c])
        #vigra.show()
        c+=1

        if c >= maxSamples:
            break
    print "DSET",len(raws)
    return raws, osegs, gts



def computeFeatureNormalization(h5file, trainSamples):

    allMinVals = []
    allMaxVals = []
    for i in trainSamples:
        group_i = h5file['item_%d'%i]
        rawFeat  = group_i['raw_edge_features']

        nFeat = rawFeat.shape[1]
        qs= [0.0001*100.0, 0.9999*100.0]
        
        psarray = numpy.percentile(rawFeat,q=qs,axis=0)#,interpolation='nearest')
        #assert psarray.shape[0] == 2
        minVals = psarray[0]#,:]
        maxVals = psarray[1]#,:]
        assert minVals.shape[0] == nFeat
        assert maxVals.shape[0] == nFeat

        allMinVals.append(minVals)
        allMaxVals.append(maxVals)

    allMinVals = numpy.array(allMinVals)
    allMaxVals = numpy.array(allMaxVals)

    assert allMinVals.shape[0] == len(trainSamples)
    assert allMaxVals.shape[0] == len(trainSamples)
    assert allMinVals.shape[1] == nFeat
    assert allMaxVals.shape[1] == nFeat

    minVals = numpy.min(allMinVals,axis=0)
    maxVals = numpy.max(allMaxVals,axis=0)
    assert minVals.shape[0] == nFeat
    assert maxVals.shape[0] == nFeat

    gNorm = h5file.create_group('normalization')
    gNorm['minVals'] = minVals
    gNorm['maxVals'] = maxVals


def trainRF(h5file, trainSamples, outFolder):

    minVals = h5file['normalization']['minVals'][:]
    maxVals = h5file['normalization']['maxVals'][:]

    allMinVals = []
    allMaxVals = []

    gts = []
    feats =[]
    for i in trainSamples:

        group_i = h5file['item_%d'%i]
        rawFeat  = group_i['raw_edge_features'][:]
        gt       = group_i['projected_gt'][:]
        uv       = group_i['uv'][:]

        edgeGt = numpy.zeros(uv.shape[0],dtype='uint32')
        lu = gt[uv[:,0]]
        lv = gt[uv[:,1]]
        edgeGt[lu!=lv] = 1

        gts.append(edgeGt)
        feats.append(rawFeat)

    gt = numpy.concatenate(gts)[:,None]
    feats = numpy.concatenate(feats,axis=0)

    print "gt   ",gt.shape,gt.dtype
    print "feats",feats.shape, feats.dtype


    rf = vigra.learning.RandomForest(treeCount=100, sample_classes_individually=True)
    oob = rf.learnRF(feats,gt)

    rf.writeHDF5(workingDir+"rf.h5",'rf')

    print "oob",oob

def predictRF(h5file, outFolder):

    minVals = h5file['normalization']['minVals'][:]
    maxVals = h5file['normalization']['maxVals'][:]
    nItems = long(numpy.array(h5file['meta']['nItems']))
    allMinVals = []
    allMaxVals = []

    gts = []
    feats =[]

    rf = vigra.learning.RandomForest(outFolder+'rf.h5','rf')
    for i in range(nItems):
        print i
        group_i = h5file['item_%d'%i]
        rawFeat  = group_i['raw_edge_features'][:]
        probs = rf.predictProbabilities(rawFeat)[:,1]
        probs = probs[:,None]

        eprobs = (probs + 0.001)/1.001
       

        # normalize 
        feat = normalizeRawFeat(rawFeat, minVals, maxVals).astype('float64')
        featP = feat.copy()
        featP *= eprobs

        allFeats = [
            feat,
            featP,
            probs,
            (probs)**2.0,
            ((probs + 1)/2.0)**2.0
        ]

        #for f in allFeats:
        #    print f.shape

        allFeats = numpy.concatenate(allFeats,axis=1)
        group_i['sssmart_edge_features'] = allFeats


def normalizeRawFeat(rawFeat, minF, maxF):
    t = maxF-minF
    nFeat = rawFeat.copy()
    nFeat -= minF[None,:]
    nFeat[nFeat<0] = 0
    nFeat /= t[None,:]
    nFeat[nFeat>1.0] = 1.0
    return nFeat

def makeInfernoDset(h5file, samples, weightVector = None):

    minVals = h5file['normalization']['minVals'][:]
    maxVals = h5file['normalization']['maxVals'][:]

    nFeatures = h5file['item_0']['sssmart_edge_features'].shape[1]

    #nFeatures = len(maxVals)
    nSamples = len(samples)

    ParaMcModel = inferno.models.ParametrizedMulticutModel

    if weightVector is None:
        weightVector = inferno.learning.WeightVector(nFeatures, 0.0)

    mVec = ParaMcModel.modelVector(nSamples)
    hammings = ParaMcModel.lossFunctionVector('edgeHamming',nSamples)
    vis = ParaMcModel.lossFunctionVector('variationOfInformation',nSamples)
    gts = ParaMcModel.confMapVector(nSamples)


    for i,si in enumerate(samples):
        print i,si
        group_i = h5file['item_%d'%si]

        # load data
        feat  = group_i['sssmart_edge_features'][:].astype('float64')
        projectedGt = group_i['projected_gt'][:]
        edges = group_i['uv'][:].astype('uint64')
        nVar = long(numpy.array(group_i['nNodes']))
        nodeSizes  = group_i['nodeSizes'][:]
        # normalize 
        #feat = normalizeRawFeat(rawFeat, minVals, maxVals).astype('float64')

        model = mVec[i]
        hamming = hammings[i]
        vi = vis[i]
        gt = gts[i]

        # assign model
        model._assign(nVar, edges, feat, weightVector)

        # assign gt
        gt.assign(model)
        gt.view()[:] = projectedGt[:]

        # assign edge hamming
        edgeLossWeight = model.factorMap('float64', 1.0)
        hamming.assign(model, edgeLossWeight, rescale=1.0/edges.shape[0],underseg=1.0,overseg=1.0)

        # assign vi
        sizeMap = model.variableMap('float64', 1.0)
        sizeMap.view()[:] = nodeSizes[:]
        vi.assign(model, sizeMap)

    return mVec, hammings, vis, gts, weightVector



if __name__ == "__main__":

    workingDir = "/home/tbeier/datasets/struct_learn/neuroproof_2d/"
    neuroproofRoot = "/home/tbeier/src/neuroproof_examples/"



   






    if False:
        raws, osegs, gts = extract2d(neuroproofRoot)
        f = h5py.File(workingDir+"dataset.h5",'w')
        make_ds.makeDs(raws=raws, gts=gts, oversegs=osegs, h5file = f)
        f.close()
    if False:
        f = h5py.File(workingDir+"dataset.h5",'r+')
        computeFeatureNormalization(f, list(range(30)) )
        f.close()

    if False:
        f = h5py.File(workingDir+"dataset.h5",'r+')
        trainRF(f, list(range(30)),workingDir)
        f.close()

    if False:
        f = h5py.File(workingDir+"dataset.h5",'r+')
        predictRF(f,workingDir)
        f.close()


    if True:
        f = h5py.File(workingDir+"dataset.h5",'r')
        trainingSamples = list(range(30))
        testSamples = list(range(30,50))

        mVec, hammings, vis, gts, weightVector = makeInfernoDset(h5file=f, samples=trainingSamples)

        # hamming dset
        ParaMcModel = inferno.models.ParametrizedMulticutModel

        if True:
            dset = inferno.learning.dataset.vectorDataset(mVec, hammings, gts)
            LossAugmentedModel = ParaMcModel.lossAugmentedModelClass('edgeHamming')

            factory = inferno.inference.multicutFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)',numThreads=1)
            lossFactory = inferno.inference.multicutFactory(LossAugmentedModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)',numThreads=8)
            
            
            # make the learner
            learner = inferno.learning.learners.subGradient(dset, maxIterations=40,n=0.05, c=0.1, m=0.2, nThreads=1)

            # do the learning
            with vigra.Timer("learn"):
                learner.learn(lossFactory, weightVector, factory)


            print "total loss ", dset.averageLoss(factory,0)*len(dset)



        if True:

            dset = inferno.learning.dataset.vectorDataset(mVec, vis, gts)

            factory = inferno.inference.multicutFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)',numThreads=1)
            ehcFactory = factory#inferno.inference.ehcFactory(ParaMcModel)

            
            # make the learner
            seed = long(time.clock()*1000000.0)
            print "seed", seed,time.clock()




          



            with vigra.Timer("approx"):
                nper = 1
                sg = inferno.learning.learners.stochasticGradient
                learner = sg(dset, maxIterations=2, nPertubations=nper, sigma=1.0, seed=42,
                                   n=10.0)
                learner.learn(ehcFactory, weightVector)

            #sys.exit(1)

            if False:
                nper = 3
                print "np ",nper
                sg = inferno.learning.learners.stochasticGradient
                learner = sg(dset, maxIterations=200, nPertubations=nper, sigma=1.0, seed=43,
                                   n=10.0)
                learner.learn(ehcFactory, weightVector)


                nper = 4
                print "np ",nper
                sg = inferno.learning.learners.stochasticGradient
                learner = sg(dset, maxIterations=200, nPertubations=nper, sigma=1.0, seed=44,
                                   n=10.0)
                learner.learn(ehcFactory, weightVector)

                nper = 5
                print "np ",nper
                sg = inferno.learning.learners.stochasticGradient
                learner = sg(dset, maxIterations=200, nPertubations=nper, sigma=1.0, seed=45,
                                   n=10.5)
                learner.learn(ehcFactory, weightVector)




        mVec, hammings, vis, gts, weightVector = makeInfernoDset(h5file=f, samples=testSamples, weightVector=weightVector)
        dset = inferno.learning.dataset.vectorDataset(mVec, hammings, gts)
        dset.updateWeights(weightVector)

        factory = inferno.inference.multicutFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)',numThreads=1)


        for testSampleIndex in testSamples:

            print "testSamples",testSampleIndex
            group_i = f['item_%d'%testSampleIndex]




           
            model = dset.model(0)
            
            solver = factory.create(model)
            verboseVisitor = inferno.inference.verboseVisitor(model)
            solver.infer(verboseVisitor.visitor())
            conf = solver.conf()

            arg = conf.view().astype('uint32')


            raw = group_i['pixel_raw'][:]
            gt = group_i['pixel_gt'][:]
            overseg = group_i['pixel_overseg'][:]

            gg = vigra.graphs.gridGraph(raw.shape[0:2])
            rag = vigra.graphs.regionAdjacencyGraph(graph=gg, labels=overseg)
            print arg.min(), arg.max()
            rag.show(raw, arg + 10)
            vigra.show()