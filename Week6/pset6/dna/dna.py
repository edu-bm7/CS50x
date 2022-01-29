from sys import argv
import csv
import re


max_agatc = 0
max_aatg = 0
max_tatc = 0


def main():
    # Check if the user input the correct number of argumments.
    if not (len(argv) == 3):
        print("Usage: python dna.py data.csv sequence.txt")

    else:
        # read the csv file into a list.
        with open(argv[1], mode='r') as csv_file:
            csv_list = csv.reader(csv_file)

            # Open the txt sequence file.
            with open(argv[2], mode='r') as sequence:
                # So we can use the sequence as a string in max() function.
                string = ''
                new_list = []

                # The list of SRT values.
                find_list = []

                for item in sequence:
                    string += item

                # load the csv_list to new_list
                for row in csv_list:
                    new_list.append(row)

                # Searchs for the SRT values in the txt file provided by the user.
                for each in new_list[0][1:]:

                    # RegEx to fing optionally(?:) the sequence that repeats 1 or more times(+) into the sequence provided by the user.
                    repeat = re.findall(rf"(?:{each})+", string)

                    # If repeat list is not empity.
                    if repeat:

                        # Get the largest repeat from a list of repeatable [max()] and
                        # then get the lenght of the string and divide it by the number of characters the substring have.
                        max_repeat = len(max(repeat, key=len)) // len(each)
                        find_list.append(str(max_repeat))

                    else:
                        continue
                # Compare new_list with the list of the SRT values found in the csv file.
                for x in new_list:
                    if find_list == x[1:]:
                        print(x[0])
                        return

                    else:
                        continue
                print('No match')


# Check if this is the main file, if so call main.
if __name__ == "__main__":
    main()