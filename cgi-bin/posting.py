#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
import errno
import urllib

input_data = os.environ['QUERY_STRING'].split('=')
file_path = './resource/database/posting/post'

if input_data[0] != "id":
    print('Status: 400 Bad Request\r')
    print('Content-Type: text/html; charset=utf-8\r\n\r')
    sys.exit()

# 400
try:
    if os.path.exists(file_path + input_data[1]):
        file_path += input_data[1]
except IndexError:
    print('Status: 400 Bad Request\r')
    print('Content-Type: text/html; charset=utf-8\r\n\r')
    sys.exit()

try:
    f = open(file_path, 'r')
except IOError as e:
    if e.errno == 13:
        print('Status: 403 Forbidden\r')
        print('Content-Type: text/html; charset=utf-8\r\n\r')
    # 404
    elif e.errno == 2:
        print('Status: 404 Not Found\r')
        print('Content-Type: text/html; charset=utf-8\r\n\r')
    sys.exit()
line = f.readline()

post_data = line.split('&')
post_data_kv = []
for i in range(len(post_data)):
    post_data_kv.append(post_data[i].split('='))

print('Status: 200 OK\r')
print('Content-Type: text/html; charset=utf-8\r\n\r')

print('<!DOCTYPE html>')
print('<html lang="en">')
print('<head>')
print('    <meta charset="UTF-8">')
print('    <meta name="viewport" content="width=device-width, initial-scale=1.0">')
print('    <title>Webserv 42 Seoul</title>')
print('    <link rel="stylesheet" href="../html/post.css">')
print('</head>')
print('<body>')
print('    <h1 class="head">Welcome to My Webserv</h1>')
print('    <div class="post-container">')
print('        <div class="title">{}</div>'.format(urllib.unquote(post_data_kv[0][1]).replace('+',' ')))
print('        <div class="content">{}</div>'.format(urllib.unquote(post_data_kv[1][1]).replace('+',' ')))
print('    </div>')
print('<button onclick="sendDeleteRequest()" class="delete-button">Delete</button>')
print('<script>')
print('function sendDeleteRequest() {')
print('    const url = "/cgi-bin/delete.py?id={}";'.format(input_data[1]))
print('    const xhr = new XMLHttpRequest();')
print('    xhr.open("DELETE", url, true);')
print('    xhr.setRequestHeader("Content-Type", "application/json");')
print('    xhr.onload = function () {')
print('        if (xhr.status >= 200 && xhr.status < 300) {')
print('            console.log("DELETE request successful");')
print('            window.location.href = "/cgi-bin/delete.py?id={}";'.format(input_data[1]))
print('        } else {')
print('            console.error("Error occurred");')
print('            document.body.innerHTML = xhr.responseText;')  # 응답 본문을 페이지에 표시
print('        }')
print('    };')
print('    xhr.onerror = function () {')
print('        console.error("Network error occurred");')
print('    };')
print('    xhr.send();')
print('}')
print('</script>')

print('</body>')
print('</html>')

# print('<button onclick="sendDeleteRequest()" class="delete-button">Delete</button>')
# print('<script>')
# print('function sendDeleteRequest() {')
# print('    const url = "/cgi-bin/delete.py?id={}";'.format(input_data[1]))
# print('    const xhr = new XMLHttpRequest();')
# print('    xhr.open("DELETE", url, true);')
# print('    xhr.setRequestHeader("Content-Type", "application/json");')
# print('    xhr.onload = function () {')
# print('        if (xhr.status >= 200 && xhr.status < 300) {')
# print('            console.log("DELETE request successful");')
# print('            window.location.href = "/cgi-bin/delete.py?id={}";'.format(input_data[1]))
# print('        } else if (xhr.status >= 400) {')
# print('            console.error("Fuuuuuck");')
# print('            window.location.href = "/cgi-bin/delete.py?id={}";'.format(input_data[1]))
# print('        } else {')
# print('            console.error("Fuuuuuck");')
# print('            window.location.href = "/cgi-bin/image_500.html";')
# print('        }')
# print('    };')
# print('    xhr.onerror = function () {')
# print('        console.error("Network error occurred");')
# print('    };')
# print('    xhr.send();')
# print('}')
# print('</script>')

# print('</body>')
# print('</html>')
