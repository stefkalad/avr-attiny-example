from termcolor import colored


# INPUT_FILE_PATH_01 = '../files/fw-blinky-jakub.txt'
# INPUT_FILE_PATH_02 = '../files/fw-blinky-jakub-avr-objcopy-output.bin'
# INPUT_FILE_PATH_03 = '../files/fw-blinky-jakub.hex'

INPUT_FILE_PATH_01 = '../files/production/app.txt'
INPUT_FILE_PATH_03 = '../files/production/app.hex'

# INPUT_FILE_PATH_01 = '../files/fw-blinky-trial-01/fw-blinky.txt'
# INPUT_FILE_PATH_02 = '../files/fw-blinky-trial-01/fw-blinky.bin'
# INPUT_FILE_PATH_03 = '../files/fw-blinky-trial-01/fw-blinky.hex'
# INPUT_FILE_PATH_04 = '../files/fw-blinky-trial-01/fw-blinky_fromhex.bin'

BYTES_TO_DISPLAY = 16



def read_and_convert_text_file_from_TPI(file_path):
    decimal_values = []
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            # Split the line by ":" to separate the parts
            parts = line.split(":")
            if len(parts) > 1:
                hex_values = parts[1].strip().split()
                # Convert each hex value to decimal and add it to the list
                decimal_values.extend([int(value, 16) for value in hex_values])
    return decimal_values



def read_and_convert_binary(file_path):
    with open(file_path, 'rb') as file:
        byte_data = file.read()
        decimal_values = [int(byte) for byte in byte_data]
        return decimal_values

        
HEX_DATA_LEN_IDX = 0
HEX_DATA_ADDR_IDX = 2
HEX_DATA_RECORD_IDX = 6
HEX_DATA_IDX = 8

def read_and_convert_hex(file_path):
    data_bytes = []
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            # Skip lines that do not start with ':'
            if line[0] != ':':
                continue
            # Extract the data bytes from the line
            hex_data = line[1:].strip()
            hex_data_len = int(hex_data[HEX_DATA_LEN_IDX:HEX_DATA_ADDR_IDX], 16)

            data_bytes.extend([int(hex_data[HEX_DATA_IDX+i:HEX_DATA_IDX+i+2], 16) for i in range(0, hex_data_len*2, 2)])
    return data_bytes



def compare_lists(list1, list2):
    max_length = max(len(list1), len(list2))
    for i in range(max_length):
        item1 = list1[i] if i < len(list1) else None
        item2 = list2[i] if i < len(list2) else None

        if item1 == item2:
            print(colored(f'{item1: <7}', 'green'), end=' ')
        else:
            print(colored(f'{item1: <3}', 'red') if item1 is not None else '   ', end=' ')
            print(colored(f'{item2: <3}', 'red') if item2 is not None else '   ', end=' ')
        
        if (i + 1) % 16 == 0:
            print()
    print()

def print_nicely(decimal_output):
  for i,value in enumerate(decimal_output):
      print(f'{value:{3}}', end= ' ' if (i+1) % BYTES_TO_DISPLAY != 0  else '\r\n')
      
  print()

if __name__ == "__main__":
  output1 = read_and_convert_text_file_from_TPI(INPUT_FILE_PATH_01)
  # print_nicely(output1)
  
  # output2 = read_and_convert_binary(INPUT_FILE_PATH_02)
  # # print_nicely(output2)
  # compare_lists(output1, output2)

  output3 = read_and_convert_hex(INPUT_FILE_PATH_03)
  # print_nicely(output3)

  # output4 = read_and_convert_binary(INPUT_FILE_PATH_02)
  # print_nicely(output4)
  compare_lists(output1, output3)
