import inferno
import numpy

gm = inferno.models.GeneralDiscreteGraphicalModel(6, 2)

####################################################
# POTTS
####################################################
# add potts functions
vals = numpy.array([1.0,2.0,3.0], dtype="float32")
vtiRange = gm.addPottsValueTables(2, vals)
# add potts factors
vis = numpy.array([[0,1],[0,2],[1,2]])
fiRange = gm.addFactors(vtiRange, vis)


####################################################
# EXPLICIT UNARY
####################################################
# add explicit unary functions
vals = numpy.random.rand(3,2)
vtiRange = gm.addExplicitValueTables(2, vals)
# add potts factors
vis = numpy.array([0, 1, 2])
fiRange = gm.addFactors(vtiRange, vis)

####################################################
# EXPLICIT 2-ORDER
####################################################
# add explicit unary functions
vals = numpy.random.rand(3,2,2)
vtiRange = gm.addExplicitValueTables(2, vals)
# add potts factors
vis = numpy.array([[0,1],[0,2],[1,2]])
fiRange = gm.addFactors(vtiRange, vis)

####################################################
# EXPLICIT 6-ORDER
####################################################
# add explicit unary functions
vals = numpy.random.rand(1,2,2,2,2,2,2)
vtiRange = gm.addExplicitValueTables(2, vals)
# add potts factors
vis = numpy.array([[0,1,2,3,5,6]])
fiRange = gm.addFactors(vtiRange, vis)