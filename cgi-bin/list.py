#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import urllib

folder_path = './resource/database/posting/'

file_list = os.listdir(folder_path)

post_files = [file for file in file_list if file.startswith('post')]

try:
    for file_name in post_files:
        f = open(folder_path + file_name, 'r')
        f.close()
except:
    print('Status: 403 Forbidden\r')
    print('Content-Type: text/html; charset=utf-8\r\n\r')
    sys.exit()

print('Status: 200 OK\r')
print('Content-Type: text/html; charset=utf-8\r\n\r')

print('<!DOCTYPE html>')
print('<html lang="en">')
print('<head>')
print('    <meta charset="UTF-8">')
print('    <meta name="viewport" content="width=device-width, initial-scale=1.0">')
print('    <title>Webserv 42 Seoul</title>')
print('    <link rel="stylesheet" href="../html/list.css">')
print('</head>')
print('<body>')
print('    <h1 class="header">Welcome to My Webserv</h1>')
print('    <ul>')

if len(post_files) == 0 :
    print('<div class="no-post">No post!</div>')
else:
    for file_name in post_files:
        with open(folder_path + file_name, 'r') as f:
            line = f.readline().split('&')
            print('        <a href="/cgi-bin/posting.py?id={}">{}</a>'.format(file_name[4:], urllib.unquote(line[0][6:]).replace('+',' ')))
print('    </ul>')
print('        <a href="/cgi-bin/write.py" class="post-button">post</a>')
print('    <img src="/image/pepe3.jpeg" alt="Example Image" style="width:300px;">')
print('</body>')
print('</html>')
