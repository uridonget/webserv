#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os

input_data = os.environ['QUERY_STRING'].split('=')
file_path = './resource/database/posting/post'

if input_data[0] != "id":
    print('Status: 400 Bad Request\r')
    print('Content-Type: text/html; charset=utf-8\r\n\r')
    sys.exit()

try:
    if os.path.exists(file_path + input_data[1]):
        os.remove(file_path + input_data[1])
except:
    print('Status: 400 Bad Request\r')
    print('Content-Type: text/html; charset=utf-8\r\n\r')
    sys.exit()

print('Status: 200 OK\r')
print('Content-Type: text/html; charset=utf-8\r\n\r')

print('<!DOCTYPE html>')
print('<html lang="en">')
print('<head>')
print('    <meta charset="UTF-8">')
print('    <meta name="viewport" content="width=device-width, initial-scale=1.0">')
print('    <title>cgi page</title>')
print('    <link rel="stylesheet" href="../html/delete.css">')
print('</head>')
print('<body>')
print('    <h1 class="header">Welcome to My Webserv</h1>')
print('    <h1 class="success">Delete successfully</h1>')
print('    <form class="post-container" method="GET" action="/cgi-bin/list.py">')
print('        <button type="submit" class="go-button">go to list</button>')
print('    </form>')
print('</body>')
print('</html>')
