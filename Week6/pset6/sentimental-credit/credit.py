from cs50 import get_string

card = get_string("Type your card (only numbers): ")

alist = []
blist = []
clist = []


def validate(c):
    for each in c[-2::-2]:
        intvalue = int(each)
        intvalue *= 2
        each2 = str(intvalue)
        alist.append(each2)
    for value in alist:
        for digit in value:
            nvalue = int(digit)
            clist.append(nvalue)
    v = sum(clist)
    if not (len(c) % 2 == 0):
        for leftover in c[::2]:
            ivalue = int(leftover)
            blist.append(ivalue)

    else:
        for leftover in c[1::2]:
            ivalue = int(leftover)
            blist.append(ivalue)

    w = sum(blist) + v
    final = str(w)
    return int(final[-1])


if (len(card) == 13) or (len(card) == 16) and card[0] == "4":
    if (validate(card) == 0):
        print("VISA")
    else:
        print("INVALID")

elif((len(card) == 16) and (not card[0] == 4)):
    if card[0:2] in ("51", "52", "53", "54", "55"):
        if (validate(card) == 0):
            print("MASTERCARD")
        else:
            print("INVALID")
    else:
        print("INVALID")

elif(len(card) == 15):
    if card[0:2] in ("34", "37"):
        if (validate(card) == 0):
            print("AMEX")
        else:
            print("INVALID")
    else:
        print("INVALID")
else:
    print("INVALID")