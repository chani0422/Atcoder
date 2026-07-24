%matplotlib inline
import matplotlib.pyplot as plt
plt.figure(figsize=(4,4))
for i in range(3):
  LX=[0,2]
  LY=[i,i]
  plt.plot(LX,LY,color="red")
plt.xlim(-1,3)
plt.ylim(-1,3)
plt.show()