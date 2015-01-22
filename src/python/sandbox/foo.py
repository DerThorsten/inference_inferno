import inferno
import numpy

shape = [20, 30]
nVar = shape[0] * shape[1]

model = inferno.explicitDiscreteModel(nVar,0, 1)


vi=0;
for x in range(shape[0]):
	for y in range(shape[1]):


		unaries = numpy.random.random([2])
		f = inferno.twoClassUnary(vi, unaries[0], unaries[1])

	vi+=1




conf = inferno.SmallVecDiscreteLabel()
conf.append(0)
print f.arity()
print f[conf]