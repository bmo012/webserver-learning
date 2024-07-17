#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <regex.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080
// 100 MB
#define BUFFER_SIZE 104857600

void get_file_extension(const char *filename) {
    const char *dot = strrchr(file_name, ".");
    if (!dot || dot == file_name) {
	return "";
    } 
    return dot + 1;
}

const *get_mime_type(const char *file_ext) {
    if (strcasecmp(file_ext, "html") == 0 || strcasecmp(file_ext, "htm") == 0) {
	return "text/html";
    } else if (strcasecmp(file_ext, "txt") == 0) {
	return "text/plain";
    } else if (strcasecmp(file_ext, "jpg") == 0 || strcasecmp(file_ext, "jpeg") == 0) {
	return "image/jpeg";
    } else if (strcasecmp(file_ext, "png") == 0) {
	return "image/png";
    } else {
	return "application/octet-stream";
    }
}

char *url_decode(const char *src) {
    size_t src_len = strlen(src);
    char *decoded = malloc(src_len + 1);
    size_t decoded_len = 0;

    // the ascii value is encoded as two hex values behind a % sign
    for (size_t i = 0; i < src_len; i++) {
	if (src[i] == '%' && i + 2 < src_len) {
	    int hex_val;
	    sscanf(src + i + 1, "%2x", &hex_val);
	    decoded[decoded_len++] = hex_val;
	    i += 2;
	} else {
	    decoded[decoded_len++] = src[i];
	}
    }
    
    // add null terminator
    decoded[decoded_len] = '\0';
    return decoded;
}

void build_http_response(const char *file_name, const char *file_ext,
			char *response, size_t *response_len) {
    // build HTTP header
    const char *mime_type = get_mime_type(file_ext);
    char *header = (char *)malloc(BUFFER_SIZE * sizeof(char));
    snprintf(header, BUFFER_SIZE, "HTTP/1.1 200 OK\r\n"
	    "Content-Type: %s\r\n\r\n", mime_type);

    // if file does not exist then respond with 404
    int file_id = open(file_name, O_RDONLY);
    if (file_fd == -1) {
	snprintf(response, BUFFER_SIZE, "HTTP/1.1 404 Not Found\r\n"
		"Content-Type: text/plain\r\n\r\n404 Not Found");
	*response_len = strlen(response);
	return;
    }

    // get file size for Content-Length
    struct stat file_stat;
}

void *handle_client(void *arg) {
    int client_fd = *((int *)arg);
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    
    // receive request data from client and store into buffer
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) {
	// check if HTTP 1 GET request
	regex_t regex;
	regcomp(&regex, "^GET /([^ ]*) HTTP/1", REG_EXTENDED);
	regmatch_t matches[2];

	if (regexec(&regex, buffer, 2, matches, 0) == 0) {
	    // extract filename from request then decode URL
	    buffer[matches[1].rm_eo] = '\0';
	    const char *url_encoded_file_name = buffer + matches[1].rm_so;
	    char *file_name = url_decode(url_encoded_file_name);

	    // get the file extension
	    char file_ext[32];
	    strcpy(file_ext, get_file_extension(file_name);	    	    

	    char *response = (char *)malloc(BUFFER_SIZE * 2 * sizeof(char));
	    size_t response_len;
	    build_http_response(file_name, file_ext, response, &response_len);
	    

	}

    }

}

int main(int argc, char *argv[]) {
    // fd stands for file descriptor, is what socket() returns
    int server_fd; 
    struct sockaddr_in server_addr;

    // create server socket
    // SOCK_STREAM indicates a TCP socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	perror("socket failed");
	exit(EXIT_FAILURE);
    }
	    
    // config socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // bind socket to port
    if (bind(server_fd
	   (struct sockaddr *)&server_addr,
	    sizeof(server_addr)) < 0) {
	perror("bind failed");
	exit(EXIT_FAILURE);
    }

    // listen for connections
    if (listen(server_fd, 10) < 0) {
	perror("bind_failed");
	exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);
    while (1) {
	// client info
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	// client_fd is a pointer to our new memory allocated in the heap
	int *client_fd = malloc(sizeof(int));
    
	// accept client connection
	if ((*client_fd = accept(server_fd,
				(struct sockaddr *)&client_addr,
				&client_addr_len)) < 0) {
	    perror("accept failed");
	    continue;
	}

	// create a new thread to handle client requests
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, handle_client, (void *)client_fd);
	pthread_detach(thread_id);

    }

    close(server_fd);
    return 0;
}
