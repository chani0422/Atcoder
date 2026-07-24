%matplotlib inline
import matplotlib.pyplot as plt
year=[2019,2020,2021]
math=[40,50,90]
eng=[50,55,80]
sci=[60,70,85]
ave=[]
for i in range(3):
  ave.append((math[i]+eng[i]+sci[i])/3)
# plt.figure(figsize=(6,4))
plt.plot(year,ave,marker="s",label="Average",color="pink")
plt.legend(loc="upper left")
plt.xlabel("year")
plt.ylabel("score")
plt.title("Evaluation")
plt.xlim(2018.8,2021.2)
plt.ylim(48,87)
plt.xticks(year)
plt.show()