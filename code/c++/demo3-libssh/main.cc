#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/tcp.h>


void demo1_main(int argc, char** argv);

//int verify_knownhost(ssh_session session)
//{
//	enum ssh_known_hosts_e state;
//	unsigned char *hash = NULL;
//	ssh_key srv_pubkey = NULL;
//	size_t hlen;
//	char buf[10];
//	char *hexa;
//	char *p;
//	int cmp;
//	int rc;
//
//	rc = ssh_get_server_publickey(session,&srv_pubkey);
//	if(rc<0){return -1;}
//
//	state = ssh_session_is_known_server(session);
//
//	switch(state)
//	{
//		case SSH_KNOWN_HOSTS_OK:
//			break;
//		case SSH_KNOWN_HOSTS_CHANGED:
//			fprintf(stderr,"Host key for server changed:it is now:\n");
//			ssh_print_hexa("public key hash", hash, hlen);
//			ssh_clean_pubkey_hash(&hash);
//			return -1;
//	}
//	ssh_clean_pubkey_hash(&hash);
//	return 0
//}

// 服务器认证
int serverAuth(ssh_session m_session)
{
    int state = SSH_SERVER_ERROR;
    int rc = SSH_ERROR;
    ssh_key srv_pubkey = {0};
    state = ssh_is_server_known(m_session);

	// libssh低版本 
    rc = ssh_get_publickey(m_session, &srv_pubkey);
    
//	rc = ssh_get_server_publickey(session, &srv_pubkey);

    if (SSH_OK != rc)
    {
        return SSH_SERVER_ERROR;
    }
    unsigned char *hash = NULL;
    size_t hlen = 0;
    char *hexa = NULL;
    rc = ssh_get_publickey_hash(srv_pubkey, SSH_PUBLICKEY_HASH_SHA1, &hash, &hlen);
    ssh_key_free(srv_pubkey);
    if (0 != rc)
    {
        return SSH_SERVER_ERROR;
    }
    if (0 >= hlen)
    {
        return SSH_SERVER_ERROR;
    }

    switch(state)
    {
        case SSH_SERVER_KNOWN_OK: break;
        case SSH_SERVER_KNOWN_CHANGED:
            hexa = ssh_get_hexa(hash, hlen);
            ssh_string_free_char(hexa);
            break;
        case SSH_SERVER_FOUND_OTHER: break;
        case SSH_SERVER_FILE_NOT_FOUND:
        case SSH_SERVER_NOT_KNOWN:
            ssh_write_knownhost(m_session);
            state = SSH_SERVER_KNOWN_OK;
            break;
        case SSH_SERVER_ERROR: break;
    }
    ssh_clean_pubkey_hash(&hash);
    return state;

}

// 用户认证
int userAuth(ssh_session session)
{
	ssh_options_set(session,SSH_OPTIONS_USER,"junn");
	int rc = ssh_userauth_password(session,NULL,"123456");
	return rc;
}

void demo1_main(int argc, char** argv)
{
	ssh_session my_ssh_session;
	int verbosity = SSH_LOG_PROTOCOL;
	int port = 22;

	my_ssh_session = ssh_new();

	if(my_ssh_session == NULL){exit(-1);}

	ssh_options_set(my_ssh_session,SSH_OPTIONS_HOST,"localhost");
	ssh_options_set(my_ssh_session,SSH_OPTIONS_LOG_VERBOSITY,&verbosity);
	ssh_options_set(my_ssh_session,SSH_OPTIONS_PORT,&port);

int rc;
	rc = ssh_connect(my_ssh_session);
	
	if (rc!=SSH_OK)
	{
		fprintf(stderr,"连接到localhost:%s \n时出错",ssh_get_error(my_ssh_session));
		exit(-1);
	}

//	verify_knownhost(my_ssh_session);

	ssh_disconnect(my_ssh_session);
	ssh_free(my_ssh_session);

}


int show_remote_processes(ssh_session session)
{
	ssh_channel channel;
	int rc;
	char buffer[256];
	int nbytes;

	channel = ssh_channel_new(session);

	rc = ssh_channel_open_session(channel);

	rc = ssh_channel_request_exec(channel,"ps aux");

	nbytes = ssh_channel_read(channel,buffer,sizeof(buffer),0);

	while (nbytes > 0)
	{
		if(write(1,buffer,nbytes) != (unsigned int)nbytes)
		{
			ssh_channel_close(channel);
			ssh_channel_free(channel);
			return SSH_ERROR;
		}
		nbytes = ssh_channel_read(channel,buffer,sizeof(buffer),0);
	}

	if (nbytes < 0)
	{
			ssh_channel_close(channel);
			ssh_channel_free(channel);
			return SSH_ERROR;
	}

	ssh_channel_send_eof(channel);
	ssh_channel_close(channel);
	ssh_channel_free(channel);
	return SSH_OK;
}



int main(int argc, char** argv)
{
	ssh_session my_ssh_session = ssh_new();
	if(my_ssh_session == NULL){exit(-1);}
	ssh_options_set(my_ssh_session,SSH_OPTIONS_HOST,"localhost");
	int port=22;
	int rc=ssh_connect(my_ssh_session);

	if (rc != SSH_OK)
	{
		fprintf(stderr,"连接到localhost:%s \n时出错",ssh_get_error(my_ssh_session));
		exit(-1);
	}

	serverAuth(my_ssh_session);
	userAuth(my_ssh_session);
	show_remote_processes(my_ssh_session);



	return 0;
}
