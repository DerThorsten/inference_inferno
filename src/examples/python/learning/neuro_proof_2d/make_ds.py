import vigra
from vigra import graphs
import numpy

from functools import partial










def addNonLinearity(edgeFeatures):
    squares = edgeFeatures**2
    logOfSquares = numpy.log(squares+1.0)
    edgeFeatures = numpy.concatenate([edgeFeatures,squares,logOfSquares],axis=1)
    return edgeFeatures

def makeDs(raws, gts, oversegs, h5file, makeProducts=True,):

    group_meta = h5file.create_group("meta")
    group_meta['nItems'] = len(raws)
    for i, (raw,gt,overseg) in enumerate(zip(raws,gts,oversegs)):

        group_i = h5file.create_group("item_%d"%i)


        group_i['pixel_raw'] = raw
        group_i['pixel_gt'] = gt
        group_i['pixel_overseg'] = overseg

        print raw.shape
        gg = graphs.gridGraph(raw.shape[0:2])
        #print "GG early ",gg.shape
        rag = graphs.regionAdjacencyGraph(graph=gg, labels=overseg)
        assert rag.nodeNum == overseg.max() +1
        assert rag.maxNodeId == overseg.max()

        uv = rag.uvIds()
        group_i['uv'] = uv
        group_i['nNodes'] = rag.nodeNum
        #print gt.dtype,gt.shape
        baseGraphGt,quality = rag.projectBaseGraphGt(gt)

        group_i['projected_gt'] = baseGraphGt
        nodeSize = rag.nodeSize()
        group_i['nodeSizes'] = nodeSize
        #print baseGraphGt
        if False:
            #rag.show(raw)
            #vigra.show()

            rag.show(raw,labels=baseGraphGt)
            vigra.show()

        log = vigra.filters.laplacianOfGaussian
        stEv = vigra.filters.structureTensorEigenvalues
        hessianEv = vigra.filters.hessianOfGaussianEigenvalues
        gaussSmooth = vigra.filters.gaussianSmoothing
        gausGradMag = vigra.filters.gaussianGradientMagnitude
        
        filters = [
            #[ partial(log,scale=s) for s in (1.0,2.0,3.0,4.0) ],
            [ partial(stEv,innerScale=s, outerScale=s*1.5) for s in (1.0,2.0,3.0) ],
            [ partial(stEv,innerScale=s, outerScale=s*2.5) for s in (1.0,2.0,3.0) ],
            [ partial(hessianEv,scale=s) for s in (1.0,2.0,4.0) ],
            [ partial(gaussSmooth,sigma=s) for s in (1.0,4.0) ],
            [ partial(gausGradMag,sigma=s) for s in (1.0,2.0,4.0) ]
        ]

        filters = [item for sublist in filters for item in sublist]
        
        allEdgeFeatures = []
        for i,f in enumerate(filters):
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

                    ggEdgeMap = graphs.implicitMeanEdgeMap(gg, resCS)
                    edgeFeatures = rag.accumulateEdgeFeatures(ggEdgeMap)

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

                    fl = [
                        d[:,None],
                        #s[:,None],
                        #p[:,None],
                        #de[:,None],
                        ade[:,None],
                        edgeFeatures[:,None],
                        #maxAduv[:,None],
                        #minAduv[:,None],
                        #dmima[:,None]
                    ]
                    ef = numpy.concatenate(fl,axis=1)
                    allEdgeFeatures.append(ef)

        edgeFeatures = numpy.concatenate(allEdgeFeatures,axis=1)
        #edgeFeatures = addNonLinearity(edgeFeatures)
        edgeFeatures = edgeFeatures
        print edgeFeatures.shape

        group_i['raw_edge_features'] = edgeFeatures

#raws=[raw]
#gts=[gt]
#
#seg,nseg=vigra.analysis.slicSuperpixels(raw,seedDistance=5,intensityScaling=60.0)
#oversegs=[seg]
#makeDs(raws=raws, gts=gts,oversegs=oversegs)

