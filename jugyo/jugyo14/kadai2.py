%matplotlib inline
import matplotlib.pyplot as plt
plt.figure(figsize=(4,4))
for i in range(10):
  for j in range(10):
    plt.plot(i,j,marker="o",label="Average",color="blue",linestyle="none")
plt.xlim(-1,10)
plt.ylim(-1,10)
plt.show()