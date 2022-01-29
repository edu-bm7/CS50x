from cs50 import get_string

s = get_string("Text: ")
# Word(w), Letter(l) and Sentence(stn) variables
w = 0
l = 0
snt = 0

# Iterate over every char in the string.
for letter in s:
    if letter == " ":
        w += 1
    if letter.isalpha():
        l += 1
    elif letter in ("!", "?", "."):
        snt += 1
    else:
        continue

# So the last word can be added.
w += 1

# The average of letters per 100 words(L).
L = round((l * 100.00 / w), 2)

# The average of sentences per 100 words(S).
S = round((snt * 100.00 / w), 2)

# The algorithm.
cl = 0.0588 * L - 0.296 * S - 15.8

if cl >= 16:
    print("Grade 16+")
elif cl < 1:
    print("Before Grade 1")
elif (cl * 10) % 10 >= 5:
    print(f"Grade {int(cl) + 1}")
else:
    print(f"Grade {int(cl)}")