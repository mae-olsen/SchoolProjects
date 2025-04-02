import socket

def fetch_file_from_server():
    # Server details
    host = "gaia.cs.umass.edu"
    path = "/wireshark-labs/alice.txt"
    port = 80  # HTTP uses port 80
    
    # Create a TCP socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        # Connect to the server
        client_socket.connect((host, port))
        print(f"Connected to {host} on port {port}")
        
        # Construct the HTTP GET request
        request = f"GET {path} HTTP/1.1\r\n" \
                  f"Host: {host}\r\n" \
                  f"Connection: close\r\n\r\n"
        
        # Send the request
        client_socket.send(request.encode())
        print(f"Sent HTTP GET request:\n{request}")
        
        # Receive the response
        response = b""
        while True:
            data = client_socket.recv(1024)
            if not data:
                break
            response += data
        
        # Separate HTTP headers and body
        headers, _, body = response.partition(b"\r\n\r\n")
        
        # Print HTTP headers
        print("HTTP Headers:\n")
        print(headers.decode())
        
        # Save the body (file content) to a local file
        with open("alice.txt", "wb") as f:
            f.write(body)
        print("\nFile content saved to 'alice.txt'.")
    
    finally:
        # Close the connection
        client_socket.close()
        print("Connection closed.")

# Fetch the file
fetch_file_from_server()
