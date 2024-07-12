#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

input_data = sys.stdin.readline().upper().split('&')
input_data_kv = []
for i in range(len(input_data)):
    input_data_kv.append(input_data[i].split('='))

print('<!DOCTYPE html>')
print('<html lang="en">')
print('<head>')
print('    <meta charset="UTF-8">')
print('    <title>cgi page</title>')
print('</head>')
print('<body>')
print('    <h1>this is input data</h1>')
for i in range(len(input_data_kv)):
    print('    <p>{} : {}</p>'.format(input_data_kv[i][0], input_data_kv[i][1]))
print('</body>')
print('</html>')
