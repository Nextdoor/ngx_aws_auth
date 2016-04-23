#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#define AWS_S3_VARIABLE "s3_auth_token"
#define AWS_DATE_VARIABLE "aws_date"

static void* ngx_http_aws_auth_create_loc_conf(ngx_conf_t *cf);
static char* ngx_http_aws_auth_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static char* ngx_http_aws_sign(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

typedef struct {
    ngx_str_t access_key;
    ngx_str_t key_scope;
    ngx_str_t signing_key;
} ngx_http_aws_auth_conf_t;


static ngx_command_t  ngx_http_aws_auth_commands[] = {
    { ngx_string("aws_access_key"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_aws_auth_conf_t, access_key),
      NULL },

    { ngx_string("aws_key_scope"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_aws_auth_conf_t, key_scope),
      NULL },

    { ngx_string("aws_signing_key"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_aws_auth_conf_t, signing_key),
      NULL },
  
    { ngx_string("aws_sign"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_http_aws_sign,
      0,
      0,
      NULL },
  
      ngx_null_command
};

static ngx_http_module_t  ngx_http_aws_auth_module_ctx = {
    NULL,                     /* preconfiguration */
    NULL,                                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    ngx_http_aws_auth_create_loc_conf,     /* create location configuration */
    ngx_http_aws_auth_merge_loc_conf       /* merge location configuration */
};


ngx_module_t  ngx_http_aws_auth_module = {
    NGX_MODULE_V1,
    &ngx_http_aws_auth_module_ctx,              /* module context */
    ngx_http_aws_auth_commands,                 /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

static void *
ngx_http_aws_auth_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_aws_auth_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_aws_auth_conf_t));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }

    return conf;    
}

static char *
ngx_http_aws_auth_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_aws_auth_conf_t *prev = parent;
    ngx_http_aws_auth_conf_t *conf = child;

    ngx_conf_merge_str_value(conf->access_key, prev->access_key, "");
    ngx_conf_merge_str_value(conf->key_scope, prev->key_scope, "");
    ngx_conf_merge_str_value(conf->signing_key, prev->signing_key, "");

    return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_aws_proxy_sign(ngx_http_request_t *r)
{
    /* TODO: sign and add headers in here.
       See ngx_http_add_header in ngx_http_headers_filter_module.c
    */
    return NGX_OK;
}

static char *
ngx_http_aws_sign(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t  *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_aws_proxy_sign;

    return NGX_CONF_OK;
}

/* 
 * vim: ts=4 sw=4 et
 */

