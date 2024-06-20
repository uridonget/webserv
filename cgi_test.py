#!/usr/bin/env python3
import cgi

# CGI 매개변수 가져오기
form = cgi.FieldStorage()

# 입력 데이터 가져오기
input_data = form.getvalue('input_data')

# 기존 HTML 파일 읽기
with open('index.html', 'r', encoding='utf-8') as file:
    html_file = file.read()

# HTML 페이지 생성
html_content = """

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Hello World CGI</title>
</head>
<body>
    <h1>Hello World!</h1>
</body>
</html>
""".format(input_data)

# HTML 페이지 출력
print(html_content)
