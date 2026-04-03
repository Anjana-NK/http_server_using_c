#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1048576

const char *get_file_extension(const char *file_name) { // Function to get .html, .png, etc
    const char *dot = strrchr(file_name, '.');
    if (!dot || dot == file_name) {
        return "";
    }
    return dot + 1;
}

// ---------- MIME TYPE ----------
const char *get_mime_type(const char *file_ext) {
    if (_stricmp(file_ext, "html") == 0 || _stricmp(file_ext, "htm") == 0)
        return "text/html";
    else if (_stricmp(file_ext, "txt") == 0)
        return "text/plain";
    else if (_stricmp(file_ext, "jpg") == 0 || _stricmp(file_ext, "jpeg") == 0)
        return "image/jpeg";
    else if (_stricmp(file_ext, "png") == 0)
        return "image/png";
    else
        return "application/octet-stream";
}

// ---------- URL DECODE ----------
char *url_decode(const char *src) {
    size_t src_len = strlen(src);
    char *decoded = malloc(src_len + 1);
    size_t decoded_len = 0;

    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == '%' && i + 2 < src_len) {
            int hex_val;
            sscanf(src + i + 1, "%2x", &hex_val);
            decoded[decoded_len++] = (char)hex_val;
            i += 2;
        } else {
            decoded[decoded_len++] = src[i];
        }
    }

    decoded[decoded_len] = '\0';
    return decoded;
}

// ---------- BUILD RESPONSE ----------
void build_http_response(const char *file_name,
                         const char *file_ext,
                         char *response,
                         size_t *response_len) {

    const char *mime_type = get_mime_type(file_ext);

    FILE *file = fopen(file_name, "rb");

    if (!file) {
        snprintf(response, BUFFER_SIZE,
                 "HTTP/1.1 404 Not Found\r\n"
                 "Content-Type: text/plain\r\n\r\n"
                 "404 Not Found");
        *response_len = strlen(response);
        return;
    }

    int header_len = snprintf(response, BUFFER_SIZE,
                              "HTTP/1.1 200 OK\r\n"
                              "Content-Type: %s\r\n\r\n",
                              mime_type);

    *response_len = header_len;

    size_t bytes_read;
    while ((bytes_read = fread(response + *response_len, 1,
                               BUFFER_SIZE - *response_len, file)) > 0) {
        *response_len += bytes_read;
    }

    fclose(file);
}

// ---------- HANDLE CLIENT ----------
DWORD WINAPI handle_client(LPVOID arg) {
    SOCKET client_fd = *(SOCKET *)arg;
    free(arg);

    char *buffer = malloc(BUFFER_SIZE);

    int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);

    if (bytes_received > 0) {

        char method[10], path[1024];

        sscanf(buffer, "%s %s", method, path);

        if (strcmp(method, "GET") == 0) {

            char *encoded = path + 1;

            char *file_name = url_decode(encoded);

            if (strlen(file_name) == 0) {
                free(file_name);
                file_name = _strdup("index.html");
            }

            char file_ext[32];
            strcpy(file_ext, get_file_extension(file_name));

            char *response = malloc(BUFFER_SIZE);
            size_t response_len;

            build_http_response(file_name, file_ext, response, &response_len);

            send(client_fd, response, (int)response_len, 0);

            free(response);
            free(file_name);
        }
    }

    closesocket(client_fd);
    free(buffer);
    return 0;
}

// ---------- MAIN ----------
int main() {
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, 10) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Server running on http://localhost:%d\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        int addr_len = sizeof(client_addr);

        SOCKET *client_fd = malloc(sizeof(SOCKET));

        *client_fd = accept(server_fd,
                            (struct sockaddr *)&client_addr,
                            &addr_len);

        if (*client_fd == INVALID_SOCKET) {
            printf("Accept failed\n");
            free(client_fd);
            continue;
        }

        CreateThread(NULL, 0, handle_client, client_fd, 0, NULL);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}