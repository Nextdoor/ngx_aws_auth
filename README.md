# AWS proxy module

This nginx module can proxy requests to authenticated S3 backends using Amazon's
V4 authentication API. The first version of this module was written for the V2
authentication protocol and can be found in the 1.x branch.

## License
This project uses the same license as ngnix does i.e. the 2 clause BSD / simplified BSD / FreeBSD license

## Usage example

Implements proxying of authenticated requests to S3.

```nginx
  server {
    listen     8000;

    location / {
      proxy_pass http://your_s3_bucket.s3.amazonaws.com;

      aws_access_key your_aws_access_key;
      aws_secret_key the_secret_associated_with_the_above_access_key;
      s3_bucket your_s3_bucket;

      proxy_set_header Authorization $s3_auth_token;
      proxy_set_header x-amz-date $aws_date;
    }

    # This is an example that does not use the server root for the proxy root
	location /myfiles {
	
      rewrite /myfiles/(.*) /$1 break;
      proxy_pass http://your_s3_bucket.s3.amazonaws.com/$1;

      aws_access_key your_aws_access_key;
      aws_secret_key the_secret_associated_with_the_above_access_key;
      s3_bucket your_s3_bucket;
      chop_prefix /myfiles; # Take out this part of the URL before signing it, since '/myfiles' will not be part of the URI sent to Amazon  


      proxy_set_header Authorization $s3_auth_token;
      proxy_set_header x-amz-date $aws_date;
    }

  }
```

## Security considerations
The V4 protocol does not need access to the actual secret keys that one obtains 
from the IAM service. The correct way to use the IAM key is to actually generate
a scoped signing key and use this signing key to access S3. This nginx module
requires the signing key and not the actual secret key. It is an insecure practise
to let the secret key reside on your nginx server.

Note that signing keys have a validity of just one week. Hence, they need to
be refreshed constantly. Please useyour favourite configuration management
system such as saltstack, puppet, chef, etc. etc. to distribute the signing
keys to your nginx clusters. Do not forget to HUP the server after placing the new
signing key as nginx reads the configuration only at startup time.

A standalone python script has been provided to generate the signing key

## Known limitations
The 2.x version of the module currently only has support for GET and HEAD calls. This is because
signing request body is complex and has not yet been implemented.


## Community

The project uses google groups for discussions. The group name is nginx-aws-auth. You can visit the web forum [here](https://groups.google.com/forum/#!forum/nginx-aws-auth)


## Credits
Original idea based on http://nginx.org/pipermail/nginx/2010-February/018583.html and suggestion of moving to variables rather than patching the proxy module.

Subsequent contributions can be found in the commit logs of the project.
