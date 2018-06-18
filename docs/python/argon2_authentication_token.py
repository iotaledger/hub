#!/usr/bin/env python
import argon2

#Install argon2_cffi:
#https://argon2-cffi.readthedocs.io/en/stable/installation.html

def argon2Token(message, salt):

    res = argon2.low_level.hash_secret(message, salt, time_cost=4, memory_cost=17, parallelism=1, hash_len=64, type=argon2.low_level.Type.ID)

    token = res.split("$",4)

    return token[4][:43]

def main():

    bundleHash = b"9999999999999999999999999999999999999999999999999999999D9999999999999999999999999"
    address = b"999ZZS9LS99LPKLDGHTU999999PP9KH9K9JH999999PLR99IO999T999H9999999999HHL999YU999ZBA"
    salt = b"El6vxEO4rR009/U/u70SgPa6C7GVZQzXZOUQrkMnXFI"


    print(argon2Token(bundleHash + address, salt) == b"RWw2dnhFTzRyUjAwOS9VL3U3MFNnUGE2QzdHVlpRelh")

if __name__ == '__main__':
    main()

