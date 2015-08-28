from nose.tools import assert_equals



class TestModel(unittest.TestCase)

    def python_api(model):


        for dfactor in model.factorDescriptors():
            factor = model.factor(dfactor)

        for dunary in model.unaryDescriptors():
            unary = model.unary(dunary)

        for dvariable in model.variableDescriptors():
            variableId = model.variableId(dvariable)

        for factor in model.factors():

            size = factor.size
            arity = factor.arity
            nWeights = factor.nWeights
            
            shape = factor.shape

            # check variables
            variables = factor.variables
            variables2 = [factor.variable(i) in range(arity)]
            self.assertEqual(variable, variables2)


            if arity == 2:
                v00 = factor[0,0]
                v01 = factor[0,1]

        for unary in model.unaries():
            size = factor.size
            var = factor.variable

            # check size
            self.assertEqual(size, model.nLabels(var))

            v0 = unary[0]
