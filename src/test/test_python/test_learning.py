from nose.tools import assert_equals
import unittest
import numpy

# todo FIX python path
import inferno





class TestMulticutLearning(unittest.TestCase):

    # todo replace with "hasCplex?"
    if True:
        def test_3x3_variation_of_information_stochastic_gradient(self):

            from inferno import models 
            from inferno import learning 
            from inferno.learning import loss_functions
            from inferno.learning import learners
            from inferno.learning import dataset

            ParaMcModel = models.ParametrizedMulticutModel
            def makeDset():
                nFeatures = 3
                nVar = 9
                weightVector = learning.WeightVector(nFeatures, 0.0)

                edges =[
                    [0,1],  #cut
                    [1,2],
                    [3,4],  #cut
                    [4,5],
                    [6,7],  #cut
                    [7,8],
                    [0,3],  #cut
                    [1,4],  #cut
                    [2,5],  #cut
                    [3,6],   
                    [4,7],
                    [5,8]
                ]
                edges = numpy.require(edges, dtype='uint64')
                features = [
                             [ 0.0,  1.0, 1.0],  # (0,1)  #cut
                             [-1.0,  0.0, 1.0],  # (1,2)      
                             [ 0.0,  1.0, 1.0],  # (3,4)  #cut
                             [-1.0,  0.0, 1.0],  # (4,5)      
                             [ 0.0,  1.0, 1.0],  # (6,7)  #cut
                             [-1.0,  0.0, 1.0],  # (7,8)   
                             [-1.0,  1.0, 1.0],  # (0,3)  #cut
                             [ 1.0,  1.0, 1.0],  # (1,4)  #cut
                             [ 0.0,  1.0, 1.0],  # (2,5)  #cut
                             [-1.0,  0.0, 1.0],  # (3,6)          
                             [-1.0,  0.0, 1.0],  # (4,7)      
                             [-1.0,  0.0, 1.0]   # (5,8)      
                ]
                features = numpy.require(features, dtype='float64')

                ParaMcModel = models.ParametrizedMulticutModel


                mVec = ParaMcModel.modelVector(2)
                lossFunctions = ParaMcModel.lossFunctionVector('variationOfInformation',2)
                gts = ParaMcModel.confMapVector(2)



                for i, (model ,lossFunction, gt) in enumerate(zip(mVec,lossFunctions,gts)):

                    # assign model
                    model._assign(nVar, edges, features, weightVector)


                    # assign loss function
                    sizeMap = model.variableMap('float64', 1.0)
                    lossFunction.assign(model, sizeMap)

                    # make gt
                    gt.assign(model)
                    view = gt.view()
                    view[0] = 0
                    view[1] = 1
                    view[2] = 1
                    view[3] = 3
                    view[4] = 4
                    view[5] = 4
                    view[6] = 3
                    view[7] = 4
                    view[8] = 4


                return (mVec,lossFunctions,gts), weightVector

            (mVec,lossFunctions,gts), weightVector = makeDset()


            rType = inferno.learning.L2
            regularizer = inferno.learning.Regularizer(rType, c=1.0)
            dset = dataset.vectorDataset(mVec, lossFunctions, gts, regularizer=regularizer)
            # make the learner
           
            factory = inferno.inference.multicutFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)')
            
            learner = learners.stochasticGradient(dset, nPertubations=20,
                                                  maxIterations=5,
                                                  sigma=30.0)
            learner.learn(factory, weightVector)

            #factory = inferno.inference.multicutFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)')
            self.assertEqual(dset.averageLoss(factory,0),0)


        def test_3x3_variation_of_information_igo(self):

            from inferno import models 
            from inferno import learning 
            from inferno.learning import loss_functions
            from inferno.learning import learners
            from inferno.learning import dataset

            ParaMcModel = models.ParametrizedMulticutModel
            def makeDset():
                nFeatures = 3
                nVar = 9
                weightVector = learning.WeightVector(nFeatures, 0.0)

                edges =[
                    [0,1],  #cut
                    [1,2],
                    [3,4],  #cut
                    [4,5],
                    [6,7],  #cut
                    [7,8],
                    [0,3],  #cut
                    [1,4],  #cut
                    [2,5],  #cut
                    [3,6],   
                    [4,7],
                    [5,8]
                ]
                edges = numpy.require(edges, dtype='uint64')
                features = [
                             [ 0.0,  1.0, 1.0],  # (0,1)  #cut
                             [-1.0,  0.0, 1.0],  # (1,2)      
                             [ 0.0,  1.0, 1.0],  # (3,4)  #cut
                             [-1.0,  0.0, 1.0],  # (4,5)      
                             [ 0.0,  1.0, 1.0],  # (6,7)  #cut
                             [-1.0,  0.0, 1.0],  # (7,8)   
                             [-1.0,  1.0, 1.0],  # (0,3)  #cut
                             [ 1.0,  1.0, 1.0],  # (1,4)  #cut
                             [ 0.0,  1.0, 1.0],  # (2,5)  #cut
                             [-1.0,  0.0, 1.0],  # (3,6)          
                             [-1.0,  0.0, 1.0],  # (4,7)      
                             [-1.0,  0.0, 1.0]   # (5,8)      
                ]
                features = numpy.require(features, dtype='float64')

                ParaMcModel = models.ParametrizedMulticutModel


                mVec = ParaMcModel.modelVector(2)
                lossFunctions = ParaMcModel.lossFunctionVector('variationOfInformation',2)
                gts = ParaMcModel.confMapVector(2)



                for i, (model ,lossFunction, gt) in enumerate(zip(mVec,lossFunctions,gts)):

                    # assign model
                    model._assign(nVar, edges, features, weightVector)


                    # assign loss function
                    sizeMap = model.variableMap('float64', 1.0)
                    lossFunction.assign(model, sizeMap)

                    # make gt
                    gt.assign(model)
                    view = gt.view()
                    view[0] = 0
                    view[1] = 1
                    view[2] = 1
                    view[3] = 3
                    view[4] = 4
                    view[5] = 4
                    view[6] = 3
                    view[7] = 4
                    view[8] = 4


                return (mVec,lossFunctions,gts), weightVector

            (mVec,lossFunctions,gts), weightVector = makeDset()




            rType = inferno.learning.L2
            regularizer = inferno.learning.Regularizer(rType, c=1.0)
            dset = dataset.vectorDataset(mVec, lossFunctions, gts, regularizer=regularizer)
            # make the learner
           
            factory = inferno.inference.multicutFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)')
            
            learner = learners.igo(dset, nPertubations=100,nElites=20, beta=0.99,
                                                  maxIterations=2,
                                                  sigma=10.0)
            learner.learn(factory, weightVector)

            #factory = inferno.inference.multicutFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)')
            self.assertEqual(dset.averageLoss(factory,0),0)

        def test_3x3_edge_hamming_subgradient(self):

            from inferno import models 
            from inferno import learning 
            from inferno.learning import loss_functions
            from inferno.learning import learners
            from inferno.learning import dataset

            ParaMcModel = models.ParametrizedMulticutModel
            def makeDset():
                nFeatures = 3
                nVar = 9
                weightVector = learning.WeightVector(nFeatures, 0.0)

                edges =[
                    [0,1],  #cut
                    [1,2],
                    [3,4],  #cut
                    [4,5],
                    [6,7],  #cut
                    [7,8],
                    [0,3],  #cut
                    [1,4],  #cut
                    [2,5],  #cut
                    [3,6],   
                    [4,7],
                    [5,8]
                ]
                edges = numpy.require(edges, dtype='uint64')
                features = [
                             [ 0.0,  1.0, 1.0],  # (0,1)  #cut
                             [-1.0,  0.0, 1.0],  # (1,2)      
                             [ 0.0,  1.0, 1.0],  # (3,4)  #cut
                             [-1.0,  0.0, 1.0],  # (4,5)      
                             [ 0.0,  1.0, 1.0],  # (6,7)  #cut
                             [-1.0,  0.0, 1.0],  # (7,8)   
                             [-1.0,  1.0, 1.0],  # (0,3)  #cut
                             [ 1.0,  1.0, 1.0],  # (1,4)  #cut
                             [ 0.0,  1.0, 1.0],  # (2,5)  #cut
                             [-1.0,  0.0, 1.0],  # (3,6)          
                             [-1.0,  0.0, 1.0],  # (4,7)      
                             [-1.0,  0.0, 1.0]   # (5,8)      
                ]
                features = numpy.require(features, dtype='float64')

                ParaMcModel = models.ParametrizedMulticutModel


                mVec = ParaMcModel.modelVector(2)
                lossFunctions = ParaMcModel.lossFunctionVector('edgeHamming',2)
                gts = ParaMcModel.confMapVector(2)



                for i, (model ,lossFunction, gt) in enumerate(zip(mVec,lossFunctions,gts)):

                    # assign model
                    model._assign(nVar, edges, features, weightVector)


                    # assign loss function
                    sizeMap = model.factorMap('float64', 1.0)
                    lossFunction.assign(model, sizeMap)

                    # make gt
                    gt.assign(model)
                    view = gt.view()
                    view[0] = 0
                    view[1] = 1
                    view[2] = 1
                    view[3] = 3
                    view[4] = 4
                    view[5] = 4
                    view[6] = 3
                    view[7] = 4
                    view[8] = 4


                return (mVec,lossFunctions,gts), weightVector

            (mVec,lossFunctions,gts), weightVector = makeDset()




            rType = inferno.learning.L2
            regularizer = inferno.learning.Regularizer(rType, c=10.0)
            dset = dataset.vectorDataset(mVec, lossFunctions, gts, regularizer=regularizer)
            # make the learner
            learner = learners.subGradient(dset, maxIterations=20, m=0.2)

            factory = inferno.inference.multicutFactory(ParaMcModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)')
            LossAugmentedModel = ParaMcModel.lossAugmentedModelClass('edgeHamming')
            lossFactory = inferno.inference.multicutFactory(LossAugmentedModel,workFlow='(TTC)(MTC)(IC)(CC-IFD,TTC-I)')

            # do the learning
            learner.learn(lossFactory, weightVector, factory)
            self.assertEqual(dset.averageLoss(factory,0),0)

        def test_partition_f_score_constructor(self):
            from inferno import models 
            from inferno import learning 
            from inferno.learning import loss_functions
            from inferno.learning import learners
            from inferno.learning import dataset

            nFeatures = 3
            nVar = 9
            weightVector = learning.WeightVector(nFeatures, 0.0)

            edges =[
                [0,1],  #cut
                [1,2],
                [3,4],  #cut
                [4,5],
                [6,7],  #cut
                [7,8],
                [0,3],  #cut
                [1,4],  #cut
                [2,5],  #cut
                [3,6],   
                [4,7],
                [5,8]
            ]
            edges = numpy.require(edges, dtype='uint64')
            features = [
                         [ 0.0,  1.0, 1.0],  # (0,1)  #cut
                         [-1.0,  0.0, 1.0],  # (1,2)      
                         [ 0.0,  1.0, 1.0],  # (3,4)  #cut
                         [-1.0,  0.0, 1.0],  # (4,5)      
                         [ 0.0,  1.0, 1.0],  # (6,7)  #cut
                         [-1.0,  0.0, 1.0],  # (7,8)   
                         [-1.0,  1.0, 1.0],  # (0,3)  #cut
                         [ 1.0,  1.0, 1.0],  # (1,4)  #cut
                         [ 0.0,  1.0, 1.0],  # (2,5)  #cut
                         [-1.0,  0.0, 1.0],  # (3,6)          
                         [-1.0,  0.0, 1.0],  # (4,7)      
                         [-1.0,  0.0, 1.0]   # (5,8)      
            ]
            features = numpy.require(features, dtype='float64')

            ParaMcModel = models.ParametrizedMulticutModel


            mVec = ParaMcModel.modelVector(1)
            lossFunctions = ParaMcModel.lossFunctionVector2('partitionFScore',1)
            gts = ParaMcModel.confMapVector(1)


            model = mVec[0]
            lf = inferno.learning.loss_functions.partitionFScore(model)
            assert lf.name() == 'PartitionFScore'
            lossFunctions[0] = lf
            lf_ = lossFunctions[0]
            assert lf_.name() == 'PartitionFScore'
            gt = gts[0]


            # assign model
            model._assign(nVar, edges, features, weightVector)

            


            # assign loss function
            sizeMap = model.variableMap('float64', 1.0)

            # make gt
            gt.assign(model)
            view = gt.view()
            view[0] = 0
            view[1] = 1
            view[2] = 1
            view[3] = 3
            view[4] = 4
            view[5] = 4
            view[6] = 3
            view[7] = 4
            view[8] = 4

        def test_partition_variation_of_information_constructor(self):
            from inferno import models 
            from inferno import learning 
            from inferno.learning import loss_functions
            from inferno.learning import learners
            from inferno.learning import dataset

            nFeatures = 3
            nVar = 9
            weightVector = learning.WeightVector(nFeatures, 0.0)

            edges =[
                [0,1],  #cut
                [1,2],
                [3,4],  #cut
                [4,5],
                [6,7],  #cut
                [7,8],
                [0,3],  #cut
                [1,4],  #cut
                [2,5],  #cut
                [3,6],   
                [4,7],
                [5,8]
            ]
            edges = numpy.require(edges, dtype='uint64')
            features = [
                         [ 0.0,  1.0, 1.0],  # (0,1)  #cut
                         [-1.0,  0.0, 1.0],  # (1,2)      
                         [ 0.0,  1.0, 1.0],  # (3,4)  #cut
                         [-1.0,  0.0, 1.0],  # (4,5)      
                         [ 0.0,  1.0, 1.0],  # (6,7)  #cut
                         [-1.0,  0.0, 1.0],  # (7,8)   
                         [-1.0,  1.0, 1.0],  # (0,3)  #cut
                         [ 1.0,  1.0, 1.0],  # (1,4)  #cut
                         [ 0.0,  1.0, 1.0],  # (2,5)  #cut
                         [-1.0,  0.0, 1.0],  # (3,6)          
                         [-1.0,  0.0, 1.0],  # (4,7)      
                         [-1.0,  0.0, 1.0]   # (5,8)      
            ]
            features = numpy.require(features, dtype='float64')

            ParaMcModel = models.ParametrizedMulticutModel


            mVec = ParaMcModel.modelVector(1)
            lossFunctions = ParaMcModel.lossFunctionVector2('variationOfInformation2',1)
            gts = ParaMcModel.confMapVector(1)


            model = mVec[0]
            lf = inferno.learning.loss_functions.variationOfInformation2(model)
            assert lf.name() == 'VariationOfInformation2'
            lossFunctions[0] = lf
            lf_ = lossFunctions[0]
            assert lf_.name() == 'VariationOfInformation2'
            gt = gts[0]


            # assign model
            model._assign(nVar, edges, features, weightVector)

            


            # assign loss function
            sizeMap = model.variableMap('float64', 1.0)

            # make gt
            gt.assign(model)
            view = gt.view()
            view[0] = 0
            view[1] = 1
            view[2] = 1
            view[3] = 3
            view[4] = 4
            view[5] = 4
            view[6] = 3
            view[7] = 4
            view[8] = 4

        def test_partition_rand_index_constructor(self):
            from inferno import models 
            from inferno import learning 
            from inferno.learning import loss_functions
            from inferno.learning import learners
            from inferno.learning import dataset

            nFeatures = 3
            nVar = 9
            weightVector = learning.WeightVector(nFeatures, 0.0)

            edges =[
                [0,1],  #cut
                [1,2],
                [3,4],  #cut
                [4,5],
                [6,7],  #cut
                [7,8],
                [0,3],  #cut
                [1,4],  #cut
                [2,5],  #cut
                [3,6],   
                [4,7],
                [5,8]
            ]
            edges = numpy.require(edges, dtype='uint64')
            features = [
                         [ 0.0,  1.0, 1.0],  # (0,1)  #cut
                         [-1.0,  0.0, 1.0],  # (1,2)      
                         [ 0.0,  1.0, 1.0],  # (3,4)  #cut
                         [-1.0,  0.0, 1.0],  # (4,5)      
                         [ 0.0,  1.0, 1.0],  # (6,7)  #cut
                         [-1.0,  0.0, 1.0],  # (7,8)   
                         [-1.0,  1.0, 1.0],  # (0,3)  #cut
                         [ 1.0,  1.0, 1.0],  # (1,4)  #cut
                         [ 0.0,  1.0, 1.0],  # (2,5)  #cut
                         [-1.0,  0.0, 1.0],  # (3,6)          
                         [-1.0,  0.0, 1.0],  # (4,7)      
                         [-1.0,  0.0, 1.0]   # (5,8)      
            ]
            features = numpy.require(features, dtype='float64')

            ParaMcModel = models.ParametrizedMulticutModel


            mVec = ParaMcModel.modelVector(1)
            lossFunctions = ParaMcModel.lossFunctionVector2('randIndex',1)
            gts = ParaMcModel.confMapVector(1)


            model = mVec[0]
            lf = inferno.learning.loss_functions.randIndex(model)
            assert lf.name() == 'RandIndex'
            lossFunctions[0] = lf
            lf_ = lossFunctions[0]
            assert lf_.name() == 'RandIndex'
            gt = gts[0]


            # assign model
            model._assign(nVar, edges, features, weightVector)

            


            # assign loss function
            sizeMap = model.variableMap('float64', 1.0)

            # make gt
            gt.assign(model)
            view = gt.view()
            view[0] = 0
            view[1] = 1
            view[2] = 1
            view[3] = 3
            view[4] = 4
            view[5] = 4
            view[6] = 3
            view[7] = 4
            view[8] = 4
