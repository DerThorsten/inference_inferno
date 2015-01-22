import inferno

f = inferno.twoClassUnary(0, 0.6, 0.7)

conf = inferno.SmallVecDiscreteLabel()
conf.append(0)

print f.arity()
print f[conf]