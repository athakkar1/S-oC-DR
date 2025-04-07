import socket

HOST = "0.0.0.0"
PORT = 2000

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen(1)
    print(f"Server listening on {HOST}:{PORT}")
    conn, addr = s.accept()
    with conn:
        print(f"Connected by {addr}")
        with open("received_data.bin", "wb") as f:  # Open a file in binary write mode
            while True:
                data = conn.recv(262144)
                if not data:
                    break  # Client closed the connection
                f.write(data)  # Write the received data to the file
                print("Received and wrote data size:", len(data))