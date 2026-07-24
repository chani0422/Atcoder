%matplotlib inline
import matplotlib.pyplot as plt
plt.figure(figsize=(4,4))
X=[[i,i] for i in range(10)]
Y=[[0,9] for i in range(10)]
x=[[0,9] for i in range(10)]
y=[[i,i] for i in range(10)]
for i in range(10):
  plt.plot(X[i],Y[i],color="red")
  plt.plot(x[i],y[i],color="red")
for i in range(10):
  for j in range(10):
    plt.plot(i,j,marker="o",color="blue",linestyle="none")
plt.xlim(-1,10)
plt.ylim(-1,10)
plt.show()