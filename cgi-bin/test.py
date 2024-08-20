#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os

env_vars = os.environ

input_data = sys.stdin.readline()


print('Status: 200 OK\r')
print('Content-Type: text/html; charset=utf-8\r\n\r')

print ('<!DOCTYPE html>')
print ('<html lang="en">')
print ('<head>')
print ('    <meta charset="UTF-8">')
print ('    <meta name="viewport" content="width=device-width, initial-scale=1.0">')
print ('    <title>cgi page</title>')

print ('</head>')
print ('<body>')
print ('    <h1>This is CGI</h1>')
print ('    <h1>this is input data</h1>')
print ('<h1>this is number of input data {}</h1>'.format(input_data))
for key, value in env_vars.items():
    print ('    <h1>{} : {}</h1>'.format(key, value))
print ('</body>')
print ('</html>')

sys.stdout.flush()

# while True:
#     a = 1