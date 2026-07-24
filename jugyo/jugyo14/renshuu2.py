#prog2
x=[0,11,22,33,44,55,66]
print(x)
print(x[1:4])
print(x[2:6:2])
print(x[:3])

#prog3
x=[0,11,22,33,44,55,66]
y=x[1:3]
z=x[:]
w=x
print('y',y)
print('z',z)
print('w',w)
z[2]=88
print('z',z)
print('x',x)
w[2]=99
print('w',w)
print('x',x)

p = x[2:]
print(p)