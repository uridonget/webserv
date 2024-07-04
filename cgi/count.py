#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import cgi
import cgitb
cgitb.enable()  # CGI 디버깅을 위한 설정

def main():
    # count.txt 파일 경로
    count_file_path = '/Users/haejeong/Desktop/webserv/database/count.txt'
    
    # CGI 환경 변수로 요청 메서드 확인
    request_method = os.environ.get('REQUEST_METHOD', 'GET').upper()

    if request_method == 'POST':
        # POST 요청 처리: count.txt 파일의 값을 읽고 증가시킨 후 다시 쓰기
        form = cgi.FieldStorage()
        counter_value = form.getvalue('counter')
        
        try:
            with open(count_file_path, 'r') as file:
                current_value = file.read().strip()
                current_value = int(current_value)  # 현재 값을 정수로 변환
        except (IOError, ValueError):
            current_value = 0  # 파일을 읽을 수 없거나 변환 실패 시 0으로 초기화
        
        # counter_value가 None이면 0으로 설정하고 아니면 정수로 변환
        if counter_value is None:
            counter_value = 0
        else:
            try:
                counter_value = int(counter_value)
            except ValueError:
                counter_value = 0

        new_value = current_value + counter_value  # 새로운 값을 계산
        
        # 새로운 값을 파일에 씀
        try:
            with open(count_file_path, 'w') as file:
                file.write(str(new_value))
            print("Content-Type: text/plain\n")
            print("Count updated successfully")
        except IOError as e:
            print("Content-Type: text/plain\n")
            print("Failed to update count: {}".format(str(e)))
    else:
        # GET 요청 처리: count.txt 파일 읽기
        try:
            with open(count_file_path, 'r') as file:
                count_value = file.read().strip()
                
        except IOError as e:
            count_value = "파일을 읽을 수 없습니다."
            error_message = str(e)

        # CGI 출력
        print("Content-Type: text/html\n")
        print("<!DOCTYPE html>")
        print("<html lang='ko'>")
        print("<head>")
        print("    <meta charset='UTF-8'>")
        print("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>")
        print("    <title>숫자 증가</title>")
        print("    <style>")
        print("        body {")
        print("            font-family: Arial, sans-serif;")
        print("            text-align: center;")
        print("            margin-top: 50px;")
        print("        }")
        print("        #counter {")
        print("            font-size: 2em;")
        print("            margin: 20px;")
        print("        }")
        print("        button {")
        print("            font-size: 1em;")
        print("            padding: 10px 20px;")
        print("            margin-top: 20px;")  # 버튼의 여백 추가
        print("        }")
        print("    </style>")
        print("</head>")
        print("<body>")
        print("    <div id='counter'>{}</div>".format(count_value))
        print("    <button type='button' onclick='increaseCounter()'>숫자 증가</button>")
        if 'error_message' in locals():
            print("    <p>Error: {}</p>".format(error_message))
        print("    <script>")
        print("        var counter = {};".format(count_value))
        print("        function increaseCounter() {")
        print("            counter++;")
        print("            document.getElementById('counter').innerText = counter;")
        print("            sendCounterToServer(counter);")
        print("        }")
        print("        function sendCounterToServer(counterValue) {")
        print("            var xhr = new XMLHttpRequest();")
        print("            xhr.open('POST', '/cgi-bin/count.py', true);")
        print("            xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');")
        print("            xhr.onreadystatechange = function() {")
        print("                if (xhr.readyState === 4 && xhr.status === 200) {")
        print("                    console.log('Response received:', xhr.responseText);")
        print("                    window.location.href = window.location.href; // 페이지를 다시 로드")
        print("                } else if (xhr.readyState === 4) {")
        print("                    console.error('Error:', xhr.status, xhr.statusText);")
        print("                }")
        print("            };")
        print("            xhr.send('counter=' + encodeURIComponent(counterValue));")
        print("            console.log('Request sent with counter:', counterValue);")
        print("        }")
        print("    </script>")
        print("</body>")
        print("</html>")

if __name__ == "__main__":
    main()
