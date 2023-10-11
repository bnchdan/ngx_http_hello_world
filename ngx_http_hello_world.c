#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <sys/shm.h> 
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>

#define MAX_HTML_SIZE 2048

static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t  ngx_http_hello_world_commands[] = {
  {
    ngx_string("hello_world"),
    NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
    ngx_http_hello_world,
    0,
    0,
    NULL
  },
    ngx_null_command
};

static ngx_http_module_t  ngx_http_hello_world_module_ctx = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

ngx_module_t ngx_http_hello_world_module = {
  NGX_MODULE_V1,
  &ngx_http_hello_world_module_ctx,
  ngx_http_hello_world_commands,
  NGX_HTTP_MODULE,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NGX_MODULE_V1_PADDING
};



static ngx_int_t ngx_http_hello_world_handler(ngx_http_request_t *r)
{
    char html[MAX_HTML_SIZE];
    strcat(html, "<html>\n");
    /*
    GET args examples
    */
    if ( r->method == NGX_HTTP_GET ){
	    strcat(html, "<b>Method:</b> GET<br>\n<b>Variables:</b> ");
	    if ( r->args.data){
            if ( MAX_HTML_SIZE - strlen(html) - r->args.len > 0){
		        strncat(html, (char *)r->args.data, r->args.len);
            }
	    }else{
            strcat(html, "request doesn't have variable ");
	    }
    }
    /*
    URL
    */
    if ( MAX_HTML_SIZE - strlen(html) - strlen("<br>\n<b>URL:</b> ")-r->uri.len > 0){
        strcat(html, "<br>\n<b>URL:</b> ");
        strncat(html, (char *)r->uri.data, r->uri.len);
    }

    /*
    GET IP
    */
    if ( MAX_HTML_SIZE - strlen(html) - strlen("<br>\n<b>IP:</b> ")-r->connection->addr_text.len > 0){
        strcat(html, "<br>\n<b>IP:</b> ");
        strncat(html, (char *)r->connection->addr_text.data, r->connection->addr_text.len);
    }

    /*
    User agent
    */
    if (r->headers_in.user_agent){
        if ( MAX_HTML_SIZE - strlen(html) - strlen("<br>\n<b>User-Agent:</b> ")-r->headers_in.user_agent->value.len > 0){
            strcat(html, "<br>\n<b>User-Agent:</b> ");
            strncat(html, (char *)r->headers_in.user_agent->value.data, r->headers_in.user_agent->value.len);
        }
    }else{
        if ( MAX_HTML_SIZE - strlen(html) - strlen("<br>\n<b>User-Agent:</b> not set") > 0){
            strcat(html, "<br>\n<b>User-Agent:</b> not set");
        }
    }

    /*
    Cookies
    */
#if defined(nginx_version) && nginx_version >= 1023000
    if (r->headers_in.cookie) {
        ngx_table_elt_t *h=r->headers_in.cookie;
        if ( MAX_HTML_SIZE - strlen(html) - strlen("<br>\n<b>Cookies:</b>")-strlen(h->value.data) > 0){
            strcat(html, "<br>\n<b>Cookies:</b>");
            strcat(html, h->value.data);
        }
    }
#else
    if (r->headers_in.cookies.elts){
        ngx_table_elt_t **h = r->headers_in.cookies.elts;
        if ( MAX_HTML_SIZE - strlen(html) - strlen("<br>\n<b>Cookies:</b> ")-strlen((char *)h[0]->value.data) > 0){
            strcat(html, "<br>\n<b>Cookies:</b> ");
            strcat(html, (char *)h[0]->value.data); 
        }
    }
#endif


    /*
    POST
    */
    if (r->method == NGX_HTTP_POST){
        // Read the POST body into a string
        size_t len = r->headers_in.content_length_n;
        u_char *buf = ngx_pnalloc(r->pool, len);
        if (buf == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        ngx_chain_t *cl;
        ssize_t n;
        ngx_buf_t *b;
        u_char *buf_ptr = buf;
        ngx_flag_t is_file_upload =0;
        for (cl = r->request_body->bufs; cl; cl = cl->next) {
            b = cl->buf;
            n = ngx_buf_size(b);
            if (n == 0) {
                continue;
            }

            if (n < 0) {
                return NGX_HTTP_BAD_REQUEST;
            }

            if ((size_t) n > len) {
                return NGX_HTTP_BAD_REQUEST;
            }

            if (b->in_file) {
                // Handle file upload (multipart/form-data)
                is_file_upload = 1;
                break;
            }
        
            len -= n;

            ngx_memcpy(buf_ptr, b->pos, n);
            buf_ptr += n;
        }

        if (!is_file_upload) {
            ngx_str_t post_data = {buf_ptr - buf, buf};
            if ( MAX_HTML_SIZE - strlen(html) - strlen("<br>\n<b>Method :</b> POST<br>\n<b>Post data:</b>")-post_data.len > 0){
                strcat(html, "<br>\n<b>Method :</b> POST<br>\n<b>Post data:</b>");
                strncat(html, (char *)post_data.data, post_data.len);
            }
        }else{
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "File uploaded");
        }
    }

    ngx_buf_t *b;
    ngx_chain_t out;

    if ( MAX_HTML_SIZE - strlen(html) - strlen("\n</html>\n")> 0){
        strcat(html, "\n</html>\n");
    }

    //create html response page
    r->headers_out.content_type_len = sizeof("text/html") - 1;
    r->headers_out.content_type.data = (u_char *)"text/html";
    r->headers_out.content_type_len = strlen(html);
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = strlen(html);
    ngx_http_send_header(r);

    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    out.buf = b;
    out.next = NULL;

    b->pos = (u_char *)html;
    b->last = (u_char *)html + strlen(html);
    b->memory = 1;
    b->last_buf = 1;

    return ngx_http_output_filter(r, &out);
}



static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
  ngx_http_core_loc_conf_t  *clcf;
  clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
  clcf->handler = ngx_http_hello_world_handler;
  return NGX_CONF_OK;
}
