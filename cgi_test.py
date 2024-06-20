#!/usr/bin/env python3
import cgi

# CGI 매개변수 가져오기
form = cgi.FieldStorage()

# 입력 데이터 가져오기
input_data = form.getvalue('input_data')

# 기존 HTML 파일 읽기
with open('index.html', 'r', encoding='utf-8') as file:
    html_file = file.read()

# 입력 데이터를 HTML 템플릿에 삽입
html_content = html_file.replace('{{input_data}}', input_data)

# HTTP 헤더
print("Content-Type: text/html; charset=utf-8")

# 빈 줄을 출력하여 헤더와 본문을 구분합니다.
print()

# HTML 페이지 출력
print(html_content)
