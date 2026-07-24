%matplotlib inline
import matplotlib.pyplot as plt
mX=[55,65,70]
mY=[150,160,170]
wX=[40,50,65]
wY=[145,155,165]
plt.plot(mX,mY,marker="o",label="male",color="blue")
plt.plot(wX,wY,marker="^",label="female",color="red")
plt.legend(loc="upper right")
plt.show()