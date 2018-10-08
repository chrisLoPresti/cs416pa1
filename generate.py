f = open("demofile.csv", "w")

for x in range(600000):

    if x%2 ==0:
        f.write(str(x/2)+"\n")
    else:
        f.write(str(x)+"\n")

