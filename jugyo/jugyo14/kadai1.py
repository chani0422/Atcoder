two = [[i,i*i] for i in range(1,11)]
print(two)                        
for i in range(0,10):
    if i%2 == 1:
      print(two[i])

three = [[i,i*i*i] for i in range(1,11)]
print(three)
for i in range(0,10):
  if i%2 == 0:
    print(three[i])