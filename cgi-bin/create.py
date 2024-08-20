#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os

input_data = sys.stdin.readline()
file_path = './resource/database/posting/post'

i = 1

while True :
    if os.path.exists(file_path + str(i)):
        i += 1
    else :
        with open('./resource/database/posting/post' + str(i), 'w') as f:
            f.write(input_data)
        break


print('Status: 203 Created\r')
print('Content-Type: text/html; charset=utf-8\r\n\r')

print('<!DOCTYPE html>')
print('<html lang="en">')
print('<head>')
print('    <meta charset="UTF-8">')
print('    <meta name="viewport" content="width=device-width, initial-scale=1.0">')
print('    <title>cgi page</title>')
print('    <link rel="stylesheet" href="../html/create.css">')
print('</head>')
print('<body>')
print('    <h1 class="header">Welcome to My Webserv</h1>')
print('    <h1 class="success">Upload successfully</h1>')
print('    <form class="post-container" method="GET" action="/cgi-bin/list.py">')
print('        <button type="submit" class="go-button">go to list</button>')
print('    </form>')
print('</body>')
print('</html>')
