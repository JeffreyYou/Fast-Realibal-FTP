import hashlib

str2hash = ""
f = open("data1.bin","r")
lines = f.readlines()

for i in lines:
    str2hash += i
  
result = hashlib.md5(str2hash.encode())
  
print("The hexadecimal equivalent of hash is : ")
print(result.hexdigest())