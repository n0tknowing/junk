#!/usr/bin/python3

import sys
if len(sys.argv) < 2:
    print("usage: {} <search_id QUERY>|<get_720p ID>".format(sys.argv[0]))
    print("example:")
    print("  $ animepahe.py search_id 'death note'")
    print("    5a906ee7-c281-4cf0-66c2-f9f966b33481	 37	 Death Note")
    print("    55fc6119-b733-8241-824a-88d31333d9a4	 2	 Death Note: Rewrite")
    print("  $ animepahe.py get_720p 5a906ee7-c281-4cf0-66c2-f9f966b33481")
    print("    Episode 1 = https://kwik.cx/...")
    print("    Episode 2 = https://kwik.cx/...")
    print("    ...")
    sys.exit(1)

import json
from urllib import request
from bs4 import BeautifulSoup

ua = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.74 Safari/537.36"
api_url = "https://aph.illyaveinz.workers.dev"

def get(url):
    req = request.Request(url, headers={"User-Agent": ua})
    res = request.urlopen(req)
    json_str = res.read().decode("utf-8")
    json_dec = json.JSONDecoder()
    return json_dec.decode(json_str)

def get_release_pages(anime_id):
    req = get(api_url + "/api?m=release&id=" + str(anime_id) + "&sort=episode_asc")
    if req["total"] == 0:
        return 0
    return req["last_page"]

def get_release_session(anime_id):
    ret = {}
    pages = get_release_pages(anime_id)
    for i in range(pages):
        print(api_url + "/api?m=release&id=" + str(anime_id) + "&sort=episode_asc" + "&page=" + str(i + 1))
        req = get(api_url + "/api?m=release&id=" + str(anime_id) + "&sort=episode_asc" + "&page=" + str(i + 1))
        if req["total"] == 0:
            sys.exit(1)
        data = req["data"]
        data_len = len(data)
        for j in range(data_len):
            ret[data[j]["episode"]] = data[j]["session"]
    return ret

def get_kwikpahe(session, provider="kwik"):
    ret = get(api_url + "/api?m=links&id=" + session + "&p=" + provider)
    data = ret["data"]
    data_len = len(data)
    for i in range(data_len):
        for k,v in data[i].items():
            if k == "720" and v["audio"] == "jpn":
                return v["kwik_pahewin"]
    return "https://kwik.cx"

def get_kwik(kwik_pahe):
    req = request.Request(kwik_pahe, headers={"User-Agent": ua})
    res = request.urlopen(req)
    data = res.read().decode("utf-8")
    soup = BeautifulSoup(data, "html.parser")
    src = soup.find_all("script")[0]
    src = str(src)
    l = src.split("\"")
    for i in l:
        if i.startswith("https://"):
            return i

def get_release_720p(anime_id):
    session = get_release_session(anime_id)
    for eps, ses in session.items():
        u = get_kwikpahe(ses)
        url = get_kwik(u)
        print("Episode {} = {}".format(eps, url))

def search(query):
    urlenc = query.replace(" ", "%20")
    anime = get(api_url + "/api?m=search&l=10&q=" + urlenc)
    if (anime["total"]) == 0:
        print("can't found " + sys.argv[1])
        sys.exit(1)
    data = anime["data"]
    data_len = len(data)
    for i in range(data_len):
        title = data[i]["title"]
        anime_id = data[i]["session"]
        eps = data[i]["episodes"]
        print("{}\t{}\t{}".format(anime_id, eps, title))

if sys.argv[1] == "search_id":
    search(sys.argv[2])
elif sys.argv[1] == "get_720p":
    get_release_720p(sys.argv[2])
else:
    print("error")
    sys.exit(1)
