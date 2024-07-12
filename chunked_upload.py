# -*- coding: utf-8 -*-
import socket
import os
from time import sleep

def send_chunk_via_socket(sock, chunk):
    chunk_size = "{:X}\r\n".format(len(chunk)).encode()
    sock.sendall(chunk_size + chunk + b"\r\n")

def send_chunked_request(file_path, host, port, chunk_size):
    file_size = os.path.getsize(file_path)
    print(f"File size: {file_size} bytes")
    
    with open(file_path, 'rb') as f:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((host, port))
        
        # Send the initial headers
        request_headers = (
            "POST /image/hello.html HTTP/1.1\r\n"
            "Host: {}:{}\r\n"
            "User-Agent: python-socket\r\n"
            "Accept: */*\r\n"
            "Transfer-Encoding: chunked\r\n"
            "Content-Type: application/octet-stream\r\n\r\n"
        ).format(host, port)
        
        sock.sendall(request_headers.encode())

        total_bytes_sent = 0
        chunk_count = 0

        while True:
            chunk = f.read(chunk_size)
            if not chunk:
                break
            send_chunk_via_socket(sock, chunk)
            total_bytes_sent += len(chunk)
            chunk_count += 1
            print("** CHECK ** Chunk {} sent: {} bytes".format(chunk_count, len(chunk)))

        # Send the terminating chunk
        sock.sendall(b"0\r\n\r\n")
        print("Total bytes sent: {}".format(total_bytes_sent))
        print("Total chunks sent: {}".format(chunk_count))
        print("File sent successfully: {}".format(total_bytes_sent == file_size))
        
        # 30초 동안 대기
        sleep(30)
        
        sock.close()

file_path = 'image/hello.html'
host = 'localhost'
port = 1004
chunk_size = 10

send_chunked_request(file_path, host, port, chunk_size)
