#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include "aws_functions.h"

#define AWS_S3_VARIABLE "s3_auth_token"
#define AWS_DATE_VARIABLE "aws_date"

static void* ngx_http_aws_auth_create_loc_conf(ngx_conf_t *cf);
static char* ngx_http_aws_auth_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t ngx_aws_auth_req_init(ngx_conf_t *cf);
static char* ngx_http_aws_sign(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

typedef struct {
    ngx_str_t access_key;
    ngx_str_t key_scope;
    ngx_str_t signing_key;
    ngx_str_t signing_key_decoded;
    ngx_str_t bucket_name;
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

    { ngx_string("aws_s3_bucket"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_aws_auth_conf_t, bucket_name),
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
    ngx_aws_auth_req_init,                                  /* postconfiguration */

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
    ngx_conf_merge_str_value(conf->bucket_name, prev->bucket_name, "");

    if(conf->signing_key_decoded.data == NULL)
    {
        conf->signing_key_decoded.data = ngx_pcalloc(cf->pool, 100);
        if(conf->signing_key_decoded.data == NULL)
        {
            return NGX_CONF_ERROR;
        }
    }

    if(conf->signing_key.len > 64) {
        return NGX_CONF_ERROR;
    } else {
        ngx_decode_base64(&conf->signing_key_decoded, &conf->signing_key);
    }

    return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_aws_proxy_sign(ngx_http_request_t *r)
{
    ngx_http_aws_auth_conf_t *conf = ngx_http_get_module_loc_conf(r, ngx_http_aws_auth_module);
    ngx_table_elt_t  *h;
    header_pair_t *hv;

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, 
            "About to generate signature");

    const ngx_array_t* headers_out = ngx_aws_auth__sign(r->pool, r, 
        &conf->access_key, &conf->signing_key_decoded, &conf->key_scope, &conf->bucket_name);
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, 
            "Got signature");
    
    ngx_uint_t i;
    for(i = 0; i < headers_out->nelts; i++)
    {
        hv = (header_pair_t*)((u_char *) headers_out->elts + headers_out->size * i);
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, 
                "header name %s, value %s", hv->key.data, hv->value.data);
        
        h = ngx_list_push(&r->headers_in.headers);
        if (h == NULL) {
            return NGX_ERROR;
        }

        h->hash = 1;
        h->key = hv->key;
        h->lowcase_key = hv->key.data; // TODO: enforce lower case
        h->value = hv->value;
    }

    return NGX_OK;
}

static char *
ngx_http_aws_sign(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    //ngx_http_aws_auth_conf_t  *module_conf = conf;
    //clcf->handler = ngx_http_aws_proxy_sign;

    return NGX_CONF_OK;
}

static ngx_int_t
ngx_aws_auth_req_init(ngx_conf_t *cf)
{
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_PREACCESS_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_aws_proxy_sign;

    return NGX_OK;
}
/* 
 * vim: ts=4 sw=4 et
 */

