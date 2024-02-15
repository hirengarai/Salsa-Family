# Read the content of the file
with open('aumasson.txt', 'r') as file:
    content = file.read()

# Replace blank separators with commas
updated_content = content.replace(',','')

# Write the updated content back to the file
with open('aumasson.txt', 'w') as file:
    file.write(updated_content)

    