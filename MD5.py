import hashlib

str2hash = ""
f = open("a.txt","r")
lines = f.readlines()

for i in lines:
    str2hash += i
  
result = hashlib.md5(str2hash.encode())
  
print("The hexadecimal equivalent of hash is : ", end ="")
print(result.hexdigest())