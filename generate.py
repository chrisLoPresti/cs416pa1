f = open("demofile.csv", "w")

for x in range(600000):

    f.write(str(x)+"\n")

