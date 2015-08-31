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
        raw = raw.T
        raws.append(raw)
        gts.append(vigra.analysis.labelImage(gtStack[i,:,:])-1)
        sp2d = vigra.analysis.labelImage(spStack[i,:,:])-1
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


def normalizeRawFeat(rawFeat, minF, maxF):
    t = maxF-minF
    nFeat = rawFeat.copy()
    nFeat -= minF[None,:]
    nFeat[nFeat<0] = 0
    nFeat /= t[None,:]
    nFeat[nFeat>1.0] = 1.0
    return nFeat

def makeInfernoDset(h5file, samples):

    minVals = h5file['normalization']['minVals'][:]
    maxVals = h5file['normalization']['maxVals'][:]
    nFeatures = len(maxVals)
    nSamples = len(samples)

    ParaMcModel = inferno.models.ParametrizedMulticutModel

    weightVector = inferno.learning.WeightVector(nFeatures, 0.0)

    mVec = ParaMcModel.modelVector(nSamples)
    hammings = ParaMcModel.lossFunctionVector('edgeHamming',nSamples)
    vis = ParaMcModel.lossFunctionVector('variationOfInformation',nSamples)
    gts = ParaMcModel.confMapVector(nSamples)


    for i,si in enumerate(samples):
        print i,si
        group_i = h5file['item_%d'%si]

        # load data
        rawFeat  = group_i['raw_edge_features'][:]
        projectedGt = group_i['projected_gt'][:]
        edges = group_i['uv'][:].astype('uint64')
        nVar = long(numpy.array(group_i['nNodes']))
        nodeSizes  = group_i['nodeSizes'][:]
        # normalize 
        feat = normalizeRawFeat(rawFeat, minVals, maxVals).astype('float64')

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



    def learnMc(weightVector, mVec, viVec, gtVec, hammingVecList = None):
        
        ParaMcModel = inferno.models.ParametrizedMulticutModel
        LossAugmentedModel = ParaMcModel.lossAugmentedModelClass('edgeHamming')

        getMcFactory = inferno.inference.multicutFactory
        getEhcFactory = inferno.inference.ehcFactory

        mcFactory = getMcFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)',numThreads=1)
        mcLossFactory = getMcFactory(LossAugmentedModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)',numThreads=8)

        ehcFactory = getEhcFactory(ParaMcModel)
        ehcLossFactory = getEhcFactory(LossAugmentedModel)


        getVecDest = inferno.learning.dataset.vectorDataset
        getSubGrad = inferno.learning.learners.subGradient

        
        if hammingVecList is not None:
            for hammingVec in hammingVecList:
                dset = getVecDest(mVec, hammingVec, gtVec)
                learner = getSubGrad(dset, maxIterations=10,
                                     n=0.05, c=1.0, m=0.2, 
                                     nThreads=1)
                # do the sub-gradient learning
                learner.learn(mcLossFactory, weightVector, mcFactory)

        





    if False:
        raws, osegs, gts = extract2d(neuroproofRoot)
        f = h5py.File(workingDir+"dataset.h5",'w')
        make_ds.makeDs(raws=raws, gts=gts, oversegs=osegs, h5file = f)
        f.close()
    if False:
        f = h5py.File(workingDir+"dataset.h5",'r+')
        computeFeatureNormalization(f, list(range(30)) )
        f.close()


    if True:
        f = h5py.File(workingDir+"dataset.h5",'r')
        trainingSamples = list(range(3))
        testSamples = list(range(3,50))

        mVec, hammings, vis, gts, weightVector = makeInfernoDset(h5file=f, samples=trainingSamples)

        # hamming dset
        ParaMcModel = inferno.models.ParametrizedMulticutModel

        if True:
            dset = inferno.learning.dataset.vectorDataset(mVec, hammings, gts)
            LossAugmentedModel = ParaMcModel.lossAugmentedModelClass('edgeHamming')

            factory = inferno.inference.multicutFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)',numThreads=1)
            lossFactory = inferno.inference.multicutFactory(LossAugmentedModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)',numThreads=8)
            
            
            # make the learner
            learner = inferno.learning.learners.subGradient(dset, maxIterations=10,n=0.05, c=1.0, m=0.2, nThreads=1)

            # do the learning
            with vigra.Timer("learn"):
                learner.learn(lossFactory, weightVector, factory)


            print "total loss ", dset.averageLoss(factory,0)*len(dset)



        if True:

            dset = inferno.learning.dataset.vectorDataset(mVec, vis, gts)

            factory = inferno.inference.multicutFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)',numThreads=1)
            ehcFactory = inferno.inference.ehcFactory(ParaMcModel)

            
            # make the learner
            seed = long(time.clock()*1000000.0)
            print "seed", seed,time.clock()




          



            with vigra.Timer("approx"):
                nper = 10
                sg = inferno.learning.learners.stochasticGradient
                learner = sg(dset, maxIterations=100, nPertubations=nper, sigma=1.0, seed=42,
                                   n=10.0)
                learner.learn(ehcFactory, weightVector)

            #sys.exit(1)

            if True:
                nper = 20
                print "np ",nper
                sg = inferno.learning.learners.stochasticGradient
                learner = sg(dset, maxIterations=200, nPertubations=nper, sigma=1.0, seed=43,
                                   n=10.0)
                learner.learn(ehcFactory, weightVector)


                nper = 30
                print "np ",nper
                sg = inferno.learning.learners.stochasticGradient
                learner = sg(dset, maxIterations=200, nPertubations=nper, sigma=1.0, seed=44,
                                   n=10.0)
                learner.learn(ehcFactory, weightVector)

                nper = 40
                print "np ",nper
                sg = inferno.learning.learners.stochasticGradient
                learner = sg(dset, maxIterations=200, nPertubations=nper, sigma=1.0, seed=45,
                                   n=10.5)
                learner.learn(ehcFactory, weightVector)


            print "total bevore igo ", dset.averageLoss(factory,0)*len(dset)

            igo = inferno.learning.learners.igo
            learner = igo(dset, nPertubations=300,nElites=20, beta=0.8,
                                      maxIterations=30,
                                      sigma=3.0)
            learner.learn(factory, weightVector)
            print "after igo loss ", dset.averageLoss(factory,0)*len(dset)






        #factory.create()
