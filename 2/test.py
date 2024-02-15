# Open the file in read mode
with open('441840_04012024_convt_0.25_pnbs_ChaCha_7_id_13_6.txt', 'r') as file:
    # Read the content and split it into a list of strings
    lines = file.readlines()

    # Create a list to store the numbers
    numbers = []

    # Iterate through each line
    for index, line in enumerate(lines):
        # Split each line into individual numbers
        line_numbers = line.split()

        # Ignore the last element of the second row
        if index == 0:
            line_numbers = line_numbers[:-1]

        # Convert each string number to an integer and append to the list
        numbers.extend(map(int, line_numbers))

print(numbers)
