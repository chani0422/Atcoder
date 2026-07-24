x=[i*10 for i in range(4)]
print(x)
y=[i*10+j for i in range(4) for j in range(3)]
print(y)
z=[[i*10+j for i in range(4)] for j in range(3)]
print(z) 

# p = [[i*100+10*j+i^1 for i in range(2)] for j in range(6)]
# print(p)