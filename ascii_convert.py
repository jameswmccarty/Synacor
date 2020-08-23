with open('snapshot.txt', 'r') as infile:
    for line in infile.readlines():
        line = line.split(' ')
        if len(line) == 2:
            try:
                num = int(line[1])
                if num < 176 and num > 31:
                    print(chr(num), end='')
            except:
                continue

            
