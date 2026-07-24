%matplotlib inline
import matplotlib.pyplot as plt
for i in range(3):
  for j in range(3):
    plt.plot(i,j,marker="o",label="Average",color="blue",linestyle="none")
plt.xlim(-1,3)
plt.ylim(-1,3)
plt.show()