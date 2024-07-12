#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

input_data = sys.stdin.readline().upper()

print ('<!DOCTYPE html>')
print ('<html lang="en">')
print ('<head>')
print ('    <meta charset="UTF-8">')
print ('    <meta name="viewport" content="width=device-width, initial-scale=1.0">')
print ('    <title>cgi page</title>')
print ('</head>')
print ('<body>')
print ('    <h1>Welcome to My Website</h1>')
print ('    <h1>This is CGI</h1>')
print('    <h1>this is input data</h1>')
print('    <p>{}</p>'.format(input_data))
print ('</body>')
print ('</html>')
