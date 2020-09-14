#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

static ssh_session ssh_sess = NULL;
static sftp_session sftp_sess = NULL;

int verify_knownhost(ssh_session session)
{
    enum ssh_known_hosts_e state;
    unsigned char *hash = NULL;
    ssh_key srv_pubkey = NULL;
    size_t hlen;
    char buf[10];
    char *hexa;
    char *p;
    int cmp;
    int rc;

    rc = ssh_get_server_publickey(session, &srv_pubkey);
    if (rc < 0) {
        return -1;
    }

    rc = ssh_get_publickey_hash(srv_pubkey,
            SSH_PUBLICKEY_HASH_SHA1,
            &hash,
            &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0) {
        return -1;
    }

    state = ssh_session_is_known_server(session);
    switch (state) {
        case SSH_KNOWN_HOSTS_OK:
            /* OK */
            fprintf(stdout, "Host key verify ok.\n");

            break;
        case SSH_KNOWN_HOSTS_CHANGED:
            fprintf(stderr, "Host key for server changed: it is now:\n");
            ssh_print_hexa("Public key hash", hash, hlen);
            fprintf(stderr, "For security reasons, connection will be stopped\n");
            ssh_clean_pubkey_hash(&hash);

            return -1;
        case SSH_KNOWN_HOSTS_OTHER:
            fprintf(stderr, "The host key for this server was not found but an other"
                    "type of key exists.\n");
            fprintf(stderr, "An attacker might change the default server key to"
                    "confuse your client into thinking the key does not exist\n");
            ssh_clean_pubkey_hash(&hash);

            return -1;
        case SSH_KNOWN_HOSTS_NOT_FOUND:
            fprintf(stderr, "Could not find known host file.\n");
            fprintf(stderr, "If you accept the host key here, the file will be"
                    "automatically created.\n");

            /* FALL THROUGH to SSH_SERVER_NOT_KNOWN behavior */

        case SSH_KNOWN_HOSTS_UNKNOWN:
            hexa = ssh_get_hexa(hash, hlen);
            fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
            fprintf(stderr, "Public key hash: %s\n", hexa);
            ssh_string_free_char(hexa);
            ssh_clean_pubkey_hash(&hash);
            p = fgets(buf, sizeof(buf), stdin);
            if (p == NULL) {
                return -1;
            }

            cmp = strncasecmp(buf, "yes", 3);
            if (cmp != 0) {
                return -1;
            }

            rc = ssh_session_update_known_hosts(session);
            if (rc < 0) {
                fprintf(stderr, "Error %s\n", strerror(errno));
                return -1;
            }

            break;
        case SSH_KNOWN_HOSTS_ERROR:
            fprintf(stderr, "Error %s", ssh_get_error(session));
            ssh_clean_pubkey_hash(&hash);
            return -1;
    }

    ssh_clean_pubkey_hash(&hash);
    return 0;
}

int sftp_sess_init(ssh_session session)
{
    int rc;
    sftp_sess = sftp_new(session);
    if (sftp_sess == NULL)
    {
        fprintf(stderr, "Error allocating SFTP session: %s\n",
                ssh_get_error(session));
        return SSH_ERROR;
    }

    rc = sftp_init(sftp_sess);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error initializing SFTP session: code %d.\n",
                sftp_get_error(sftp_sess));
        return rc;
    }

    return SSH_OK;
}

#define MAX_XFER_BUF_SIZE 16384
int sftp_read_async(ssh_session session, sftp_session sftp_sess, const char *src_file, const char *dest_file)
{
    int access_type;
    sftp_file file;
    char buffer[MAX_XFER_BUF_SIZE];
    int async_request;
    int nbytes;
    long counter;
    int fd = 0;
    int rc;

    access_type = O_RDONLY;
    file = sftp_open(sftp_sess, src_file, access_type, 0);
    if (file == NULL) {
        fprintf(stderr, "Can't open file for reading: %s\n", ssh_get_error(session));
        return SSH_ERROR;
    }
    sftp_file_set_nonblocking(file);

    fd = open(dest_file, O_CREAT|O_WRONLY, 0444);
    if (fd < 0) {
        fprintf(stderr, "Can't open file for writing: %s\n", strerror(errno));
        sftp_close(file);
        return SSH_ERROR;
    }

    async_request = sftp_async_read_begin(file, sizeof(buffer));
    counter = 0L;
    usleep(1000);
    if (async_request >= 0) {
        nbytes = sftp_async_read(file, buffer, sizeof(buffer),
                async_request);
    } else {
        nbytes = -1;
    }

    while (nbytes > 0 || nbytes == SSH_AGAIN) {
        if (nbytes > 0) {
            write(fd, buffer, nbytes);
            async_request = sftp_async_read_begin(file, sizeof(buffer));
        } else {
            counter++;
        }
        usleep(1000);

        if (async_request >= 0) {
            nbytes = sftp_async_read(file, buffer, sizeof(buffer),
                    async_request);
        } else {
            nbytes = -1;
        }
    }

    if (nbytes < 0) {
        fprintf(stderr, "Error while reading file: %s\n",
                ssh_get_error(session));
        sftp_close(file);
        close(fd);
        return SSH_ERROR;
    }

    printf("The counter has reached value: %ld\n", counter);

    rc = sftp_close(file);
    if (rc != SSH_OK) {
        fprintf(stderr, "Can't close the read file: %s\n",
                ssh_get_error(session));
        return rc;
    }

    rc = close(fd);
    if (rc != 0)
    {
        fprintf(stderr, "Can't close the dest file: %s\n", dest_file);
        return rc;
    }

    return rc;
}

#define MAX_XFER_BUF_SIZE 16384

int sftp_read_sync(ssh_session session, sftp_session sftp, const char *src_file, const char *dest_file)
{
    int access_type;
    sftp_file file;
    char buffer[MAX_XFER_BUF_SIZE];
    int nbytes, nwritten, rc;
    int fd;

    access_type = O_RDONLY;
    file = sftp_open(sftp, src_file, access_type, 0);
    if (file == NULL) {
        fprintf(stderr, "Can't open file for reading: %s\n", ssh_get_error(session));
        return SSH_ERROR;
    }

    fd = open(dest_file, O_CREAT|O_WRONLY, 0444);
    if (fd < 0) {
        fprintf(stderr, "Can't open file for writing: %s\n", strerror(errno));
        return SSH_ERROR;
    }

    for (;;) {
        nbytes = sftp_read(file, buffer, sizeof(buffer));
        if (nbytes == 0) {
            break; // EOF
        } else if (nbytes < 0) {
            fprintf(stderr, "Error while reading file: %s\n", ssh_get_error(session));
            sftp_close(file);
            return SSH_ERROR;
        }

        nwritten = write(fd, buffer, nbytes);
        if (nwritten != nbytes) {
            fprintf(stderr, "Error writing: %s\n", strerror(errno));
            sftp_close(file);
            return SSH_ERROR;
        }
    }

    rc = sftp_close(file);
    if (rc != SSH_OK) {
        fprintf(stderr, "Can't close the read file: %s\n", ssh_get_error(session));
        return rc;
    }

    rc = close(fd);
    if (rc != 0)
    {
        fprintf(stderr, "Can't close the dest file: %s\n", dest_file);
        return rc;
    }

    return SSH_OK;
}

int ssh_sess_init(const char *user, const char *password, const char *host, int port)
{
    int rc = 0;
    int verbosity = SSH_LOG_PROTOCOL;

    ssh_sess = ssh_new();
    if (ssh_sess == NULL)
    {
        printf("ssh_new() failed.\n");
        return(-1);
    }

    ssh_options_set(ssh_sess, SSH_OPTIONS_HOST, host);
    ssh_options_set(ssh_sess, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_set(ssh_sess, SSH_OPTIONS_PORT, &port);
    ssh_options_set(ssh_sess, SSH_OPTIONS_USER, user);

    rc = ssh_connect(ssh_sess);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error connecting to localhost: %s\n",
                ssh_get_error(ssh_sess));
        return(-1);
    }

    if (verify_knownhost(ssh_sess) < 0)
    {
        return(-1);
    }

    // Authenticate ourselves
    //password = getpass("Password: ");
    rc = ssh_userauth_password(ssh_sess, NULL, password);
    if (rc != SSH_AUTH_SUCCESS)
    {
        fprintf(stderr, "Error authenticating with password: %s\n",
                ssh_get_error(ssh_sess));
        return(-1);
    }
    return 0;
}

int main()
{
    int rc;
    char *host = "localhost";
    char *user = "zhengyagang";
    char *password = "nishishui";
    int port = 22;
    char *src_file = "/home/zhengyagang/gitlab/oru/fpga/images/linux/image.ub";
    char *dest_file = "/home/zhengyagang/image.ub";

    rc = ssh_sess_init(user, password, host, port);
    if (rc < 0)
    {
        printf("ssh session init failed.");
        goto cleanup;
    }

    if (sftp_sess_init(ssh_sess) < 0)
    {
        printf("sftp session init failed.");
        goto cleanup;
    }

    rc = sftp_read_async(ssh_sess, sftp_sess, src_file, dest_file);
    if (rc < 0)
    {
        printf("async read %s@%s:%s to local %s failed.\n", user, host, src_file, dest_file);
    }
    else
    {
        printf("async read %s@%s:%s to local %s success.\n", user, host, src_file, dest_file);
    }
cleanup:
    sftp_free(sftp_sess);
    ssh_disconnect(ssh_sess);
    ssh_free(ssh_sess);
    return rc;
}
