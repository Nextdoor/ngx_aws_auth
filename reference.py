#!/usr/bin/env python

'''
Authorization = "AWS" + " " + AWSAccessKeyId + ":" + Signature;

Signature = Base64( HMAC-SHA1( YourSecretAccessKeyID, UTF-8-Encoding-Of( StringToSign ) ) );

StringToSign = HTTP-Verb + "\n" +
	Content-MD5 + "\n" +
	Content-Type + "\n" +
	Date + "\n" +
	CanonicalizedAmzHeaders +
	CanonicalizedResource;

CanonicalizedResource = [ "/" + Bucket ] +
	<HTTP-Request-URI, from the protocol name up to the query string> +
	[ subresource, if present. For example "?acl", "?location", "?logging", or "?torrent"];

CanonicalizedAmzHeaders = <described below>
'''


def canon_resource(vhost_mode, bucket, url):
    val = "/%s" % bucket if vhost_mode else ""
    val = val+url

    return val

if __name__ == "__main__":
    print canon_resource(True, "bugait", "/shash")
    print canon_resource(False, "bugait", "/bugait/shash")
