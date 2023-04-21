#!/usr/bin/env python3

import argparse
import json
import requests
import sys
import os

#import logging
#import http.client
#http.client.HTTPConnection.debuglevel = 1

#logging.basicConfig()
#logging.getLogger().setLevel(logging.DEBUG)
#requests_log = logging.getLogger("requests.packages.urllib3")
#requests_log.setLevel(logging.DEBUG)
#requests_log.propagate = True

cached_resp = "/tmp/.gist_cache"
gist_url = "https://api.github.com/gists"
gist_err = { 304 : "(304) Not modified",
             400 : "(400) POST data invalid",
             401 : "(401) Authentication required",
             403 : "(403) Forbidden",
             404 : "(404) Content not found",
             422 : "(422) Validation failed" }

def perform_cached_request(url, header):
    resp = requests.get(url, headers=header)
    if resp.status_code != 200:
        print(gist_err[resp.status_code])
        sys.exit(1)
    ret = []
    gist = json.loads(resp.text)
    for i in range(len(gist)):
        res = {}
        res["id"] = gist[i]["id"]
        res["url"] = gist[i]["html_url"]
        res["public"] = gist[i]["public"]
        res["files"] = gist[i]["files"]
        res["desc"] = gist[i]["description"]
        ret.append(res)
    f = open(cached_resp, "w")
    f.write(resp.headers["ETag"] + "\n")
    f.write(str(ret).replace("\'", "\"")
                    .replace("True", "true")
                    .replace("False", "false")
                    .replace("None", "\"\"") + "\n")
    f.close()
    return ret

def list_gist(header):
    #cached = False
    url = gist_url + "?" + "per_page=100"
    hres = requests.head(url, headers=header)
    if hres.status_code != 200:
        print(gist_err[hres.status_code])
        sys.exit(1)
    if os.path.isfile(cached_resp):
        f = open(cached_resp, "r")
        etag = f.readline().strip()
        if etag == hres.headers["ETag"]:
    #       cached = True
            resp = json.loads(f.read().strip())
        else:
            resp = perform_cached_request(url, header)
    else:
        resp = perform_cached_request(url, header)
    for i in range(len(resp)):
        print("ID          =", resp[i]["id"])
        print("URL         =", resp[i]["url"])
        print("Public      =", resp[i]["public"])
        print("Files       =", len(resp[i]["files"]))
        print("Description =", resp[i]["desc"])
        if (i + 1) < len(resp):
            print()
    #if cached:
    #    print("cache hit")

def perform_request(header, payload):
    resp = requests.post(gist_url, headers=header, json=payload)
    if resp.status_code != 201:
        print(gist_err[resp.status_code])
        sys.exit(1)
    decoded = json.loads(resp.text)
    print(resp.status_code, decoded["html_url"])

def delete_gist(header, gist_id):
    url = gist_url + "/" + gist_id
    resp = requests.delete(url, headers=header)
    if resp.status_code != 204:
        print(gist_err[resp.status_code])
        sys.exit(1)
    print("OK")

def create_gist(header, files, description, from_stdin, is_public):
    payload = {"files" : {}, "public": not args.secret, "description": description, "public": is_public}
    if from_stdin:
        content = sys.stdin.read()
        payload["files"][files[0]] = {"content": content}
    else:
        for file in files:
            content = open(file, "r").read()
            payload["files"][file] = {"content": content}
    perform_request(header, payload)

##############################################################################

parser = argparse.ArgumentParser()
parser.add_argument("-c", "--create", help="Create gist(s), use \"-\" for STDIN", metavar="FILE", type=str, nargs="+")
parser.add_argument("-d", "--description", help="Description of the gist", type=str)
parser.add_argument("-f", "--filename", help="Custom name for the files that make up the gist (STDIN only)", type=str)
parser.add_argument("-l", "--list", help="List all your public and secret gists", action="store_true")
parser.add_argument("-s", "--secret", help="Flag indicating whether the gist is secret", action="store_true", default=False)
parser.add_argument("-t", "--token", help="GitHub token that supports read/write to gist", type=str)
parser.add_argument("-x", "--delete", help="Delete an existing gist with provided GIST_ID", metavar="GIST_ID", type=str)
parser.add_argument("-U", "--ua", help="Custom user agent, default from python requests", type=str)

args = parser.parse_args()
if not args.token:
    print("token doesn't exist")
    sys.exit(1)

headers = {"Accept": "application/vnd.github+json", "Authorization": "Bearer " + args.token}
if args.ua:
    headers["User-Agent"] = args.ua

if args.create:
    files = []
    from_stdin, is_public = False, not args.secret
    description = ""
    if args.description:
        description = args.description
    if args.create[0] == "-":
        if not args.filename:
            print("-f FILENAME must be set when standard input is used")
            sys.exit(1)
        from_stdin = True
        files.append(args.filename)
    else:
        files = args.create
    create_gist(headers, files, description, from_stdin, is_public)
elif args.list:
    list_gist(headers)
elif args.delete:
    delete_gist(headers, args.delete)
else:
    list_gist(headers)
