from cs50 import get_int

while True:
    i = get_int("Height: ")
    if (i <= 8 and i >= 1):
        break
space = " "
nspc = i - 1
gap = "  "
hsh = "#"
nhsh = 1

for height in range(i):
    hsh_ = hsh * nhsh
    spc_ = space * nspc
    print(spc_ + hsh_ + gap + hsh_)
    nspc -= 1
    nhsh += 1