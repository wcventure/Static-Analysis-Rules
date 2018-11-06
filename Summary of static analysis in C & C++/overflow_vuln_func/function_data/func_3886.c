static int http_server(struct sockaddr_in my_addr)
{
    int server_fd, tmp, ret;
    struct sockaddr_in from_addr;
    struct pollfd poll_table[HTTP_MAX_CONNECTIONS + 1], *poll_entry;
    HTTPContext *c, **cp;
    long cur_time;

    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if (server_fd < 0) {
        perror ("socket");
        return -1;
    }
        
    tmp = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp));

    if (bind (server_fd, (struct sockaddr *) &my_addr, sizeof (my_addr)) < 0) {
        perror ("bind");
        close(server_fd);
        return -1;
    }
  
    if (listen (server_fd, 5) < 0) {
        perror ("listen");
        close(server_fd);
        return -1;
    }

    http_log("ffserver started.\n");

    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    first_http_ctx = NULL;
    nb_connections = 0;
    first_http_ctx = NULL;
    for(;;) {
        poll_entry = poll_table;
        poll_entry->fd = server_fd;
        poll_entry->events = POLLIN;
        poll_entry++;

        /
        c = first_http_ctx;
        while (c != NULL) {
            int fd;
            fd = c->fd;
            switch(c->state) {
            case HTTPSTATE_WAIT_REQUEST:
                c->poll_entry = poll_entry;
                poll_entry->fd = fd;
                poll_entry->events = POLLIN;
                poll_entry++;
                break;
            case HTTPSTATE_SEND_HEADER:
            case HTTPSTATE_SEND_DATA_HEADER:
            case HTTPSTATE_SEND_DATA:
            case HTTPSTATE_SEND_DATA_TRAILER:
                c->poll_entry = poll_entry;
                poll_entry->fd = fd;
                poll_entry->events = POLLOUT;
                poll_entry++;
                break;
            case HTTPSTATE_RECEIVE_DATA:
                c->poll_entry = poll_entry;
                poll_entry->fd = fd;
                poll_entry->events = POLLIN;
                poll_entry++;
                break;
            case HTTPSTATE_WAIT_FEED:
                /
                c->poll_entry = poll_entry;
                poll_entry->fd = fd;
                poll_entry->events = 0;
                poll_entry++;
                break;
            default:
                c->poll_entry = NULL;
                break;
            }
            c = c->next;
        }

        /
        do {
            ret = poll(poll_table, poll_entry - poll_table, 1000);
        } while (ret == -1);
        
        cur_time = gettime_ms();

        /

        cp = &first_http_ctx;
        while ((*cp) != NULL) {
            c = *cp;
            if (handle_http (c, cur_time) < 0) {
                /
                log_connection(c);
                close(c->fd);
                if (c->fmt_in)
                    av_close_input_file(c->fmt_in);
                *cp = c->next;
                nb_bandwidth -= c->bandwidth;
                free(c);
                nb_connections--;
            } else {
                cp = &c->next;
            }
        }

        /
        poll_entry = poll_table;
        if (poll_entry->revents & POLLIN) {
            int fd, len;

            len = sizeof(from_addr);
            fd = accept(server_fd, (struct sockaddr *)&from_addr, 
                        &len);
            if (fd >= 0) {
                fcntl(fd, F_SETFL, O_NONBLOCK);
                /
                if (nb_connections >= nb_max_connections) {
                    close(fd);
                } else {
                    /
                    c = av_mallocz(sizeof(HTTPContext));
                    c->next = first_http_ctx;
                    first_http_ctx = c;
                    c->fd = fd;
                    c->poll_entry = NULL;
                    c->from_addr = from_addr;
                    c->state = HTTPSTATE_WAIT_REQUEST;
                    c->buffer_ptr = c->buffer;
                    c->buffer_end = c->buffer + IOBUFFER_MAX_SIZE;
                    c->timeout = cur_time + REQUEST_TIMEOUT;
                    nb_connections++;
                }
            }
        }
        poll_entry++;
    }
}
