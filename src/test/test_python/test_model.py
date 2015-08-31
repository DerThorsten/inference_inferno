from nose.tools import assert_equals
import unittest
import numpy

# todo FIX python path
import inferno





class TestMulticutLearningModel(unittest.TestCase):

    # todo replace with "hasCplex?"
    if True:
        def test_3x3_model(self):

            from inferno import models 
            from inferno import learning 
            from inferno.learning import loss_functions
            from inferno.learning import learners
            from inferno.learning import dataset

            ParaMcModel = models.ParametrizedMulticutModel



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
            # assign model
            model = mVec[0]
            model._assign(nVar, edges, features, weightVector)


            factorMap = model.factorMap('float32', 1.0)
    
            view = factorMap.view()
            self.assertEqual(view.shape[0] ,12)
            view[0] = 0
            view[1] = 1
            view[11] = 11

            view2 = factorMap.view()
            self.assertEqual(view.shape[0] ,12)
            self.assertEqual(view[0] , 0 )
            self.assertEqual(view[1] , 1 )
            self.assertEqual(view[11], 11)
