print("hello world!")

start = False

level = 1
grid = ""
max_cols = 0
max_rows = 0

MAX_LEVEL = 10

f = open("696.txt", "r")
for line in f.readlines():

    # found level data
    if not start and "#" in line:
        start = True
        grid = ""
        max_cols = 0
        max_rows = 0

    # current level finished
    if start and not "#" in line:
        print("finished! writing file {:03d}".format(level), "\n", "{:03d}|{:03d}".format(max_cols, max_rows), "\n", grid, sep="")
        wfile = open("level{:03d}.txt".format(level), "w") 
        wfile.write("{:03d}|{:03d}".format(max_cols, max_rows) + "\n" + grid) 
        wfile.close()         
        start = False
        level += 1

        if (level > MAX_LEVEL):
            break

        continue

    # reading level data
    if start:
        #print(line, end="")
        grid += line.strip() + "\n"
        if  len(line.strip()) > max_cols:
            max_cols = len(line.strip());
        
        max_rows += 1


