# My implementation of the Vernam Cipher
# It is only able to encrypt as the GeekforGeeks page showcased a faulty technique for this cipher
from string import ascii_lowercase

plaintext = str(input("Enter text: ").lower().strip())
key_input = str(input(" Enter key: ").lower().strip())

def encrypt(text: str, key: str) -> str:
    my_dict = {}
    res = []
    nums = []
    str_ = ''

    for value, item in enumerate(ascii_lowercase):
        my_dict[item] = value

    x = [my_dict[i] for i in text]  # creates list of integer values according to "my_dict"
    y = [my_dict[j] for j in key]

    for val0, val1 in zip(x,y):
        enc = (int(val0) ^ int(val1))  # XOR encryption
        res.append(enc)

    i = 0
    for _ in range(len(text)):
        enc_ = res[0+i]
        i += 1
        if enc_ >= 26:
            enc_ -= 26
        nums.append(enc_)
    
    for i in nums:
        my_dictSwap = {v: k for k, v in my_dict.items()}  # reverse dictionary: {key: value} -> {value: key}
        str_ += my_dictSwap[i]

    return str_

  
def write(filename: str) -> None:
    with open(filename, 'a') as f:
        cipher = encrypt(plaintext, key_input)
        text = (f"Encrypted Message: {cipher} \n")
        f.write(''.join(text))

if __name__ == '__main__':
    if len(plaintext) == len(key_input):
        print(encrypt(plaintext, key_input))
        try:
            write('Section_2/Encrypted_Vernam.txt')
        except FileNotFoundError:
            write('Encrypted_Vernam.txt')
    else:
        raise ValueError("Plaintext length must be Equal to of Key.")
